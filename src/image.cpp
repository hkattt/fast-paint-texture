#include <opencv2/opencv.hpp>

#include "image.hpp"

using namespace std;

cv::Mat Image::gaussian_blur(const GaussianKernel *kernel) {
    cv::Vec3b pixel, blurred_pixel;

    int width = this->image.cols, height = this->image.rows;

    // Creates a blank output image with the rewquired dimensions
    cv::Mat output_image(height, width, CV_8UC3);

    // Uses convoltion with a Gaussian kernel to compute the output pixels
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            output_image.at<cv::Vec3b>(row, col) = this->convolve(row, col, kernel);
        }
    }
    return output_image;
}

cv::Vec3b Image::convolve(int row, int col, const GaussianKernel *kernel) {
    Eigen::Vector3f pixel;
    int image_row, image_col;
    float value;

    int border = (kernel->get_len() - 1) / 2;

    // Blurrred RGB values
    float blurred_r = 0, blurred_g = 0, blurred_b = 0;

    for (int j = -border; j < border; j++) {
        for (int i = -border; i < border; i++) {
            // Image row and column indexes of the current pixel
            image_row = row + j;
            image_col = col + i;

            // Checks if the coordinates are valid
            if (image_col < 0 || image_col > this->width || image_row < 0 || image_row > this->height) {
                continue;
            }
            // Kernel value of the current pixel
            value = kernel->get_value(i + border, j + border);

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
    Eigen::Vector2f grad = Eigen::Vector2f(0);
    Eigen::Vector3f pixel;
    float grad_mag, intensity;
    int image_x, image_y;

    int border = (sobel_x->get_len() - 1) / 2;

    for (int j = -border; j < border; j++) {
        for (int i = -border; i < border; i++) {
            // Image x and y coordinates of the current pixel
            image_x = x + i;
            image_y = y + j;

            // Checks if the coordinates are valid
            if (image_x < 0 || image_x > this->width || image_y < 0 || image_y > this->height) {
                continue;
            }

            pixel = get_pixel(image_x, image_y);
            
            // Compute the intensity of the current pixel
            // The constants reflect how sensitive the human-eye is to each colour channel
            intensity = 0.2989 * pixel.x() + 0.5870 * pixel.y() + 0.1140 * pixel.z();

            // Compute gradient
            grad[0] += intensity * sobel_x->get_value(i + border, j + border);
            grad[1] += intensity * sobel_y->get_value(i + border, j + border);
        }
    }
    grad.normalize();
    // Compute the magnitude of the gradient
    grad_mag = grad.norm();

    return std::tuple<Eigen::Vector2f, float>(grad, grad_mag);
}