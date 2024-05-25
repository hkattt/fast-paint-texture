#pragma once

#include <Eigen/Eigen>

#include "kernel.hpp"
#include "stroke.hpp"

using namespace Eigen;

class Stroke;

typedef Eigen::Matrix<Eigen::Vector3f, Eigen::Dynamic, Eigen::Dynamic> RGBMatrix;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> GrayMatrix;

namespace ImageUtil {
    float get_pixel(GrayMatrix *image, int x, int y);

    void set_pixel(GrayMatrix *image, int x, int y, float colour);
    
    Eigen::Vector3f get_pixel(RGBMatrix *image, int x, int y);

    void set_pixel(RGBMatrix *image, int x, int y, Eigen::Vector3f colour);

    Eigen::Vector3f alpha_blend(Eigen::Vector3f c1, Eigen::Vector3f c2, float alpha);

    float alpha_blend(float h1, float h2, float alpha);
}

class RGBImage;

class GrayImage {
    private:
        // Dimensions of the image
        int width, height;
        // Matrix representing the image
        GrayMatrix *image;

    public:
        GrayImage() : width(0), height(0) {}

        GrayImage(int width, int height, GrayMatrix *image) {
            this->width = width;
            this->height = height;
            this->image = image;
        }

        /**
         * Constructor for Image. 
         * 
         * Creates an image with a constant colour. 
         * 
         * @param width: width of the image
         * @param height: height of the image
         * @param colour: colour to set the image to
        */
        GrayImage(int width, int height, float colour);

        GrayImage(int width, int height, cv::Mat cv_image);

        ~GrayImage() {
            delete image;
        }

        int get_width() {
            return this->width;
        }

        int get_height() {
            return this->height;
        }

        /**
         * @param x: x-coordinate of the pixel
         * @param y: y-coordinate of the pixel
         * 
         * @return: Pixel at (x, y) represented as a vector (RGB)
        */
        float get_pixel(int x, int y) {
            // Eigen uses (row, col) indicies
            return ImageUtil::get_pixel(this->image, x, y);
        }

        void set_pixel(int x, int y, float c) {
            // Eigen matrices (row, col) indicies
            ImageUtil::set_pixel(this->image, x, y, c);
        }

        GrayMatrix *get_image() {
            return this->image;
        }

        cv::Mat *to_cv_mat();

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

        RGBImage *compute_normals(HorizontalSobelKernel *sobel_x, VerticalSobelKernel *sobel_y);
};

class RGBImage {
    private:
        // Dimensions of the image
        int width, height;
        // Matrix representing the image
        RGBMatrix *image;

        /**
         * Convolves the given pixel using the Gaussian kernel.
         * 
         * @param x: x-coordiante of the pixel
         * @param col: y-coordinate of the pixel
         * @param kernel: Gaussian kernel
         * 
         * @returns: Convovled pixel
        */
        Eigen::Vector3f convolve(int x, int y, const GaussianKernel *kernel);

    public:
        RGBImage() : width(0), height(0) {}

        RGBImage(int width, int height, RGBMatrix *image) {
            this->width = width;
            this->height = height;
            this->image = image;
        }

        /**
         * Constructor for Image. 
         * 
         * Creates an image with a constant colour. 
         * 
         * @param width: width of the image
         * @param height: height of the image
         * @param colour: colour to set the image to
        */
        RGBImage(int width, int height, Eigen::Vector3f colour);

        RGBImage(int width, int height, cv::Mat cv_image);

        ~RGBImage() {
            delete image;
        }

        int get_width() {
            return this->width;
        }

        int get_height() {
            return this->height;
        }

        RGBMatrix *get_image() {
            return this->image;
        }

        void set_pixel(int x, int y, Eigen::Vector3f c) {
            // Eigen matrices (row, col) indicies
            ImageUtil::set_pixel(this->image, x, y, c);
        }

        /**
         * @param x: x-coordinate of the pixel
         * @param y: y-coordinate of the pixel
         * 
         * @return: Pixel at (x, y) represented as a vector (RGB)
        */
        Eigen::Vector3f get_pixel(int x, int y) {
            // Eigen uses (row, col) indicies
            return ImageUtil::get_pixel(this->image, x, y);
        }

        cv::Mat *to_cv_mat();

        /**
         * Performs blurring using colvolution with the provided Gaussian kernel
         * 
         * @param kernel: Gaussian kernel 
         * 
         * @return: Blurred image
        */
        RGBImage *gaussian_blur(const GaussianKernel *kernel);

        /**
         * @return: The average pixel colour of the image
        */
        Eigen::Vector3f average_colour();

        /**
         * Computes the difference between the image and the compare_image
         * 
         * @return: Matrix containing the difference betweene each pixel
        */
        GrayImage* difference(RGBImage *compare_image);

        GrayImage *luminosity();
};

