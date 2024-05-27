#include <opencv2/opencv.hpp>

#include "image.hpp"
#include "debug.hpp"

using namespace std;

namespace ImageUtil {
    float get_pixel(GrayMatrix *image, int x, int y) {
        return (*image)(y, x);
    }

    void set_pixel(GrayMatrix *image, int x, int y, float colour) {
        (*image)(y, x) = colour;
    }

    Eigen::Vector3f get_pixel(RGBMatrix *image, int x, int y) {
        return (*image)(y, x);
    }

    void set_pixel(RGBMatrix *image, int x, int y, Eigen::Vector3f colour) {
        (*image)(y, x) = colour;
    }

    Eigen::Vector3f alpha_blend(Eigen::Vector3f c1, Eigen::Vector3f c2, float alpha) {
            return (alpha * c1 + (1 - alpha) * c2).cwiseMin(255.0f).cwiseMax(0.0f);
        }

    float alpha_blend(float h1, float h2, float alpha) {
        return std::clamp(alpha * h1 + (1 - alpha) * h2, 0.0f, 255.0f);
    }
}

GrayImage::GrayImage(int width, int height, float colour) {
    this->width = width;
    this->height = height;
    GrayMatrix *image = new GrayMatrix(height, width);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            ImageUtil::set_pixel(image, x, y, colour);
        }
    }
    this->image = image;
}

GrayImage::GrayImage(int width, int height, cv::Mat cv_image) {
    this->width = width;
    this->height = height;

    GrayMatrix *image = new GrayMatrix(height, width);

    uchar pixel;
    for (int row = 0; row < cv_image.rows; row++) {
        for (int col = 0; col < cv_image.cols; col++) {
            pixel = cv_image.at<uchar>(row, col);
            
            ImageUtil::set_pixel(image, col, row, static_cast<float>(pixel));
        } 
    }
    this->image = image;
}

cv::Mat *GrayImage::to_cv_mat() {
    cv::Mat *cv_image = new cv::Mat(this->height, this->width, CV_8UC1);

    float pixel;
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            pixel = this->get_pixel(x, y);

            (*cv_image).at<uchar>(y, x) = static_cast<uchar>(pixel);
        } 
    }
    return cv_image;
}

std::tuple<Eigen::Vector2f, float> GrayImage::compute_gradient(int x, int y, HorizontalSobelKernel *sobel_x, VerticalSobelKernel *sobel_y) {
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
            if (image_x < 0 || image_x >= this->width || image_y < 0 || image_y >= this->height) {
                continue;
            }

            intensity = get_pixel(image_x, image_y);
            
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

RGBImage *GrayImage::compute_normals(HorizontalSobelKernel *sobel_x, VerticalSobelKernel *sobel_y) {
    Eigen::Vector3f normal;
    Eigen::Vector2f grad;
    float grad_mag;

    RGBMatrix *normals = new RGBMatrix(this->height, this->width);

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            // Get the unit vector of gradient (gx, gy) and gradient magnitutde (unused)
            std::tie<Eigen::Vector2f, float>(grad, grad_mag) = this->compute_gradient(x, y, sobel_x, sobel_y);

            normal = Eigen::Vector3f(-grad.x(), -grad.y(), 1).normalized();

            ImageUtil::set_pixel(normals, x, y, normal);
        }
    }
    
    return new RGBImage(this->width, this->height, normals);
}

RGBImage::RGBImage(int width, int height, Eigen::Vector3f colour) {
    this->width = width;
    this->height = height;
    RGBMatrix *image = new RGBMatrix(height, width);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            ImageUtil::set_pixel(image, x, y, colour);
        }
    }
    this->image = image;
}

RGBImage::RGBImage(int width, int height, cv::Mat cv_image) {
    this->width = width;
    this->height = height;

    RGBMatrix *image = new RGBMatrix(height, width);

    cv::Vec3b pixel;
    for (int row = 0; row < cv_image.rows; row++) {
        for (int col = 0; col < cv_image.cols; col++) {
            pixel = cv_image.at<cv::Vec3b>(row, col);
            
            // Add the current pixel to the frame buffer
            // cv::Mat origin (0, 0) starts at the top-left corner
            // cv::Vec3b stores BGR values
            ImageUtil::set_pixel(image, col, row, Eigen::Vector3f(pixel[2], pixel[1], pixel[0]));
        } 
    }
    this->image = image;
}

cv::Mat *RGBImage::to_cv_mat() {
    cv::Mat *cv_image = new cv::Mat(this->height, this->width, CV_8UC3);

    Eigen::Vector3f pixel;
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            pixel = this->get_pixel(x, y);

            // cv::Vec3b stores BGR values
            (*cv_image).at<cv::Vec3b>(y, x) = cv::Vec3b(pixel.z(), pixel.y(), pixel.x());
        } 
    }
    return cv_image;
}

RGBImage* RGBImage::gaussian_blur(const GaussianKernel *kernel) {
    // Creates a blank output image with the rewquired dimensions
    RGBImage *blurred_image = new RGBImage(this->width, this->height, new RGBMatrix(this->height, this->width));

    // Uses convoltion with a Gaussian kernel to compute the output pixels
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            blurred_image->set_pixel(x, y, this->convolve(x, y, kernel));
        }
    }
    return blurred_image;
}

Eigen::Vector3f RGBImage::convolve(int x, int y, const GaussianKernel *kernel) {
    Eigen::Vector3f pixel;
    int image_x, image_y;
    float value;

    // Blurrred RGB values
    float blurred_r = 0, blurred_g = 0, blurred_b = 0;

    for (int j = 0; j < kernel->get_len(); j++) {
        for (int i = 0; i < kernel->get_len(); i++) {
            // Image row and column indexes of the current pixel
            image_x = x + i - kernel->get_centre_x();
            image_y = y + j - kernel->get_centre_y();

            // Checks if the coordinates are valid
            if (image_x < 0 || image_x >= this->width || image_y < 0 || image_y >= this->height) {
                continue;
            }
            // Kernel value of the current pixel
            value = kernel->get_value(i, j);

            pixel = get_pixel(image_x, image_y);

            // Adds contribution from the current pixel
            blurred_r += value * pixel.x();
            blurred_g += value * pixel.y();
            blurred_b += value * pixel.z();
        }
    }
    // Returns the blurred pixel
    return Eigen::Vector3f(blurred_r, blurred_g, blurred_b);
}

GrayImage *RGBImage::luminosity() {
    GrayMatrix *luminosity = new GrayMatrix(this->height, this->width);
    
    Eigen::Vector3f pixel;
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            pixel = get_pixel(x, y);

            // Compute the intensity of the current pixel
            // The constants reflect how sensitive the human-eye is to each colour channel
            ImageUtil::set_pixel(luminosity, x, y, 0.2989 * pixel.x() + 0.5870 * pixel.y() * 0.1140 * pixel.z());
        }
    }
    return new GrayImage(this->width, this->height, luminosity);
}

Eigen::Vector3f RGBImage::average_colour() {
    Eigen::Vector3f avg = Vector3f::Zero();

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            avg += this->get_pixel(x, y);
        }
    }
    return avg / (this->width * this->height);
}

GrayImage* RGBImage::difference(RGBImage *compare_image) {
    // Ensure the images have the same dimensions
    if (this->width != compare_image->get_width() || this->height != compare_image->get_height()) {
        throw std::invalid_argument("Cannot compute the difference of images with different dimensions");
    }

    GrayMatrix *differences = new GrayMatrix(this->height, this->width);
    float difference; 
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            // Compute the distance between the current pixel values
            // | (r1, g1, b1) - (r2, g2, b2) | = sqrt((r1 - r2)^2 + (g1 - g2)^2 + (b1 - b2)^2)
            difference = (this->get_pixel(x, y) - compare_image->get_pixel(x, y)).norm();
            ImageUtil::set_pixel(differences, x, y, difference);
        }
    }
    return new GrayImage(this->width, this->height, differences);
}