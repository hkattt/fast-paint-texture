#include <opencv2/opencv.hpp>

#include "image.hpp"
#include "stroke.hpp"
#include "debug.hpp"

using namespace std;

Image* Image::gaussian_blur(const GaussianKernel *kernel) {
    cv::Vec3b pixel, blurred_pixel;

    // Creates a blank output image with the rewquired dimensions
    cv::Mat output_image(this->height, this->width, CV_8UC3);

    // Uses convoltion with a Gaussian kernel to compute the output pixels
    for (int row = 0; row < this->height; row++) {
        for (int col = 0; col < this->width; col++) {
            output_image.at<cv::Vec3b>(row, col) = this->convolve(row, col, kernel);
        }
    }
    return new Image(width, height, output_image);
}

cv::Vec3b Image::convolve(int row, int col, const GaussianKernel *kernel) {
    Eigen::Vector3f pixel;
    int image_row, image_col;
    float value;

    // Blurrred RGB values
    float blurred_r = 0, blurred_g = 0, blurred_b = 0;

    for (int j = 0; j < kernel->get_len(); j++) {
        for (int i = 0; i < kernel->get_len(); i++) {
            // Image row and column indexes of the current pixel
            image_row = row + j - kernel->get_centre_y();
            image_col = col + i - kernel->get_centre_x();

            // Checks if the coordinates are valid
            if (image_col < 0 || image_col > this->width || image_row < 0 || image_row > this->height) {
                continue;
            }
            // Kernel value of the current pixel
            value = kernel->get_value(i, j);

            pixel = get_pixel(image_col, image_row);

            // Adds contribution from the current pixel
            blurred_r += value * pixel.x();
            blurred_g += value * pixel.y();
            blurred_b += value * pixel.z();
        }
    }
    // Returns the blurred pixel
    // Note that cv::Vec3b stores (BGR) values
    return cv::Vec3b(blurred_b, blurred_g, blurred_r);
}

std::tuple<Eigen::Vector2f, float> Image::compute_gradient(int x, int y, HorizontalSobelKernel *sobel_x, VerticalSobelKernel *sobel_y) {
    Eigen::Vector2f grad = Eigen::Vector2f::Zero();
    Eigen::Vector3f pixel;
    float grad_mag, intensity;
    int image_x, image_y;

    for (int j = 0; j < sobel_x->get_len(); j++) {
        for (int i = 0; i < sobel_x->get_len(); i++) {
            // Image x and y coordinates of the current pixel
            image_x = x + i - sobel_x->get_centre_x();
            image_y = y + j - sobel_x->get_centre_y();

            // Checks if the coordinates are valid
            if (image_x < 0 || image_x > this->width || image_y < 0 || image_y > this->height) {
                continue;
            }

            pixel = get_pixel(image_x, image_y);
            
            // Compute the intensity of the current pixel
            // The constants reflect how sensitive the human-eye is to each colour channel
            intensity = 0.2989 * pixel.x() + 0.5870 * pixel.y() + 0.1140 * pixel.z();

            // Compute gradient
            grad[0] += intensity * sobel_x->get_value(i, j);
            grad[1] += intensity * sobel_y->get_value(i, j);
        }
    }
    grad.normalize();
    // Compute the magnitude of the gradient
    grad_mag = grad.norm();

    return std::tuple<Eigen::Vector2f, float>(grad, grad_mag);
}

Eigen::Vector3f Image::average_colour() {
    Eigen::Vector3f avg = Vector3f::Zero();

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            avg += this->get_pixel(x, y);
        }
    }

    return avg / (this->width * this->height);
}

cv::Mat* Image::difference(Image *compare_image) {
    // Ensure the images have the same dimensions
    if (this->width != compare_image->get_width() || this->height != compare_image->get_height()) {
        throw std::invalid_argument("Cannot compute the difference of images with different dimensions");
    }

    cv::Mat *distances = new cv::Mat(this->width, this->height, CV_32FC1);

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            // Compute the distance between the current pixel values
            // | (r1, g1, b1) - (r2, g2, b2) | = sqrt((r1 - r2)^2 + (g1 - g2)^2 + (b1 - b2)^2)
            distances->at<float>(y, x) = (this->get_pixel(x, y) - compare_image->get_pixel(x, y)).norm();
        }
    }

    return distances;
}

void Image::make_flags() {
    if (this->counters != nullptr) {
        return;
    }

    int len = this->width * this->height;

    this->counters = new int[len];
    this->total_mask = new float[len];
    this->old_colours = new Eigen::Vector3f[len];
    this->z_buffer = new int[len];

    for (int i = 0; i < len; i++) {
        counters[i] = -1;
    }

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            this->old_colours[y * this->width + x] = this->get_pixel(x, y);
        }
    }

    this->cur_counter = 0;
}

void Image::clear_z() {
    if (this->z_buffer == nullptr) {
        return;
    }
    
    for (int i = 0; i < this->width * this->height; i++) {
        this->z_buffer[i] = std::numeric_limits<int>::max();
    }
}

void Image::render_stroke(Stroke *stroke, AntiAliasedCircle *mask, int z) {
    std::vector<Eigen::Vector2f> limit = stroke->get_limit();

    this->cur_counter++;
    this->cur_colour = stroke->get_colour();

    if (limit.size() == 1) {
        this->render_stroke_point(limit[0].x(), limit[0].y(), z, mask);
        return;
    }

    for (int i = 0; i < limit.size() - 1; i++) {
        this->render_stroke_line(limit[i].x(), limit[i].y(), limit[i + 1].x(), limit[i + 1].y(), z, mask);
    }

    // TODO: What about the edge image thing???
}

void Image::render_stroke_point(int x, int y, int z, AntiAliasedCircle *mask) {
    int new_x, new_y, ind;
    float alpha;

    for (int j = 0; j < mask->get_len(); j++) {
        for (int i = 0; i < mask->get_len(); i++) {
            new_x = x + i - mask->get_centre_x();
            new_y = y + j - mask->get_centre_y();

            // Checks if the coordinates are valid
            if (new_x < 0 || new_x > this->width || new_y < 0 || new_y > this->height) {
                continue;
            }

            alpha = mask->get_value(i, j);

            ind = new_y * this->width + new_x;

            if (this->z_buffer[ind] < z) {
                continue;
            }

            if (this->counters[ind] < this->cur_counter) {
                this->counters[ind] = this->cur_counter;
                this->old_colours[ind] = get_pixel(new_x, new_y);
                this->total_mask[ind] = alpha;
                this->set_pixel(new_x, new_y, this->alpha_blend(this->cur_colour, this->old_colours[ind], alpha));
                this->set_height(new_x, new_y, this->compose_height(new_x, new_y));
                this->z_buffer[ind] = z;
            } 
            else {
                if (this->total_mask[ind] < alpha) {
                    this->total_mask[ind] = alpha;
                    this->set_pixel(new_x, new_y, this->alpha_blend(this->cur_colour, this->old_colours[ind], alpha));
                    this->set_height(new_x, new_y, this->compose_height(new_x, new_y));
                    this->z_buffer[ind] = z;
                }
            }
        }
    }
}

void Image::render_stroke_line(int x1, int y1, int x2, int y2, int z, AntiAliasedCircle *mask) {
    int xa, xb, ya, yb;
    float m, y;

    if (x1 == x2) {
        ya = std::min(y1, y2);
        yb = std::max(y1, y2);

        for (int y = ya; y < yb; y++) {
            this->render_stroke_point(x1, y, z, mask);
        }
    } 
    else {
        if (x1 < x2) {
            xa = x1;
            xb = x2;
            ya = y1;
            yb = y2;
        }
        else {
            xa = x2;
            xb = x1;
            ya = y2;
            yb = y1;
        }
        m = ((float) yb - ya) / (xb - xa);
        y = ya;

        for (int x = xa; x <= xb; x++) {
            this->render_stroke_point(x, y, z, mask);
            y += m;
        }
    }
}