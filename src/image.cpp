#include <opencv2/opencv.hpp>

#include "image.hpp"

using namespace std;

cv::Mat Image::gaussian_blur(const Kernel *kernel) {
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

cv::Vec3b Image::convolve(int row, int col, const Kernel *kernel) {
    cv::Vec3b pixel;
    int image_row, image_col;
    float value;

    // Calculate the centre of the kernel window
    int centre = (kernel->get_len() - 1) / 2;

    // Blurrred RGB values
    float blurred_r = 0, blurred_g = 0, blurred_b = 0;

    // Iterates over every point in the kernel window
    for (int j = 0; j < kernel->get_len(); j++) {
        for (int i = 0; i < kernel->get_len(); i++) {
            // Image row and column indexes of the current pixel
            image_row = row - centre + j;
            image_col = col - centre + i;

            // Checks if the coordinates are valid
            if (image_col < 0 || image_col > this->width || image_row < 0 || image_row > this->height) {
                continue;
            }
            // Kernel value of the current pixel
            value = kernel->get_value(i, j);

            pixel = this->image.at<cv::Vec3b>(image_row, image_col);

            // Adds contribution from the current pixel
            blurred_r += value * (float) pixel[0];
            blurred_g += value * (float) pixel[1];
            blurred_b += value * (float) pixel[2];
        }
    }
    // Returns the blurred pixel
    return cv::Vec3b(blurred_r, blurred_g, blurred_b);
}