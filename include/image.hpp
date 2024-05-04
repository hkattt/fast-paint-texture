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

        /**
         * Constructor for Image. 
         * 
         * Creates an image with a constant colour. 
         * 
         * @param width: width of the image
         * @param height: height of the image
         * @param colour: colour to set the image to
        */
        Image(int width, int height, Eigen::Vector3f colour) {
            this->width = width;
            this->height = height;
            cv::Mat image(height, width, CV_8UC3, cv::Scalar(colour.z(), colour.y(), colour.x()));
            this->image = image;
        }

        Image(int width, int height, cv::Mat image) {
            this->width = width;
            this->height = height;
            this->image = image;
        }

        int get_width() {
            return this->width;
        }

        int get_height() {
            return this->height;
        }

        cv::Mat get_image() {
            return this->image;
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
         * @return: Blurred image
        */
        Image gaussian_blur(const GaussianKernel *kernel);

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

        /**
         * @return: The average pixel colour of the image
        */
        Eigen::Vector3f average_colour();

        /**
         * Computes the difference between the image and the compare_image
         * 
         * @return: Matrix containing the difference betweene each pixel
        */
        cv::Mat difference(Image *compare_image);
};

