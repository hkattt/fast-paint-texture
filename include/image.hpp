#pragma once

#include "kernel.hpp"

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
        cv::Vec3b convolve(int row, int col, const Kernel *kernel);

    public:
        Image() : width(0), height(0) {}

        Image(int width, int height, cv::Mat image) {
            this->width = width;
            this->height = height;
            this->image = image;
        }

        /**
         * Performs blurring using colvolution with the provided Gaussian kernel
         * 
         * @param kernel: Gaussian kernel 
         * 
         * @returns: Matrix representing the blurred image
        */
        cv::Mat gaussian_blur(const Kernel *kernel);
};

