#pragma once

#include <Eigen/Eigen>

#include "kernel.hpp"

using namespace Eigen;

/**
 * Image class to implement image-processing functionality
*/
class Image {
    private:
        // Dimensions of the image
        int width, height;
        // Matrix representing the image
        cv::Mat image;

        /**
         * Convolves the given pixel using the Gaussian kernel.
         * 
         * @param row: row index of the pixel
         * @param col: colum index of the pixel
         * @param kernel: Gaussian kernel
         * 
         * @returns: Convovled pixel
        */
        cv::Vec3b convolve(int row, int col, const GaussianKernel *kernel);

    public:
        Image() : width(0), height(0) {}

        Image(int width, int height, cv::Mat image) {
            this->width = width;
            this->height = height;
            this->image = image;
        }

        cv::Mat get_image() {
            return image;
        }

        /**
         * @param x: x-coordinate of the pixel
         * @param y: y-coordinate of the pixel
         * 
         * @return: Pixel at (x, y) represented as a vector (RGB)
        */
        Eigen::Vector3f get_pixel(int x, int y) {
            cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
            // Note that cv::Vec3b stores (BGR) values
            return Eigen::Vector3f(pixel[2], pixel[1], pixel[0]);
        }

        /**
         * Performs blurring using colvolution with the provided Gaussian kernel
         * 
         * @param kernel: Gaussian kernel 
         * 
         * @return: Matrix representing the blurred image
        */
        cv::Mat gaussian_blur(const GaussianKernel *kernel);

        /**
         * Computes the gradient of the image at the given point
         * 
         * @param x: x-coordinate 
         * @param y: y-coordinate
         * @param sobel_x: Horizontal sobel kernel
         * @param sobel_y: Vertical sobel kernel
         * 
         * @return: Tuple containing the gradient (gx, gy) and the magnitude of the gradient
        */
        std::tuple<Eigen::Vector2f, float> compute_gradient(int x, int y, HorizontalSobelKernel *sobel_x, VerticalSobelKernel *sobel_y);
};

