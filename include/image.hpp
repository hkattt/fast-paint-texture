#pragma once

#include <opencv2/opencv.hpp>
#include <Eigen/Eigen>

#include "kernel.hpp"

using namespace Eigen;

// Eigen matrix for storing RGB (3-channel images)
typedef Matrix<Vector3f, Dynamic, Dynamic> RGBMatrix;
// Eigen matrix for storing gray-scale (single channel images)
typedef Matrix<float, Dynamic, Dynamic> GrayMatrix;

/**
 * Image utility functions not associated to a object.
*/
namespace ImageUtil {
    /**
     * @param image: Gray-scale matrix
     * @param x: x-coordinate
     * @param y: y-coordinate
     * 
     * @return: Pixel at (x, y)
    */
    float get_pixel(const GrayMatrix *image, const int x, const int y);

    /**
     * @param image: Gray-scale matrix
     * @param x: x-coordiante
     * @param y: y-coordiante
     * @param colour: colour to set
    */
    void set_pixel(GrayMatrix *image, const int x, const int y, const float colour);
    
    /**
     * @param image: Gray-scale matrix
     * @param x: x-coordinate
     * @param y: y-coordinate
     * 
     * @return: Pixel at (x, y)
    */
    Vector3f get_pixel(const RGBMatrix *image, const int x, const int y);

    /**
     * @param image: Gray-scale matrix
     * @param x: x-coordiante
     * @param y: y-coordiante
     * @param colour: colour to set
    */
    void set_pixel(RGBMatrix *image, const int x, const int y, const Vector3f colour);

    /**
     * @param c1: first colour to blend
     * @param c2: second colour to blend
     * @param alpha: alpha to blend with 
    */
    Vector3f alpha_blend(const Vector3f c1, const Vector3f c2, const float alpha);

     /**
     * @param c1: first colour to blend
     * @param c2: second colour to blend
     * @param alpha: alpha to blend with 
    */
    float alpha_blend(const float h1, const float h2, const float alpha);
}

class RGBImage;

/**
 * Object representing a gray-scale image
*/
class GrayImage {
    private:
        // Dimensions of the image
        int width, height;
        // Matrix representing the image
        GrayMatrix *image;

    public:
        GrayImage() : width(0), height(0) {}

        /**
         * Constructor for GrayImage.
         * 
         * Create a gray-scale image from the input gray matrix.
         * 
         * @param width: width of the image
         * @param height: height of the image
         * @param image: image matrix to create the image from 
        */
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
        GrayImage(const int width, const int height, const float colour);

        /**
         * Constructor for GrayImage.
         * 
         * Create a gray-scale image from the OpenCV matrix
         * 
         * @param width: width of the image
         * @param height: height of the image
         * @param cv_image: OpenCV matrix to create the image from
        */
        GrayImage(const int width, const int height, const cv::Mat cv_image);

        ~GrayImage() {
            delete image;
        }

        int get_width() const {
            return this->width;
        }

        int get_height() const {
            return this->height;
        }

        GrayMatrix *get_image() {
            return this->image;
        }

        /**
         * @param x: x-coordinate of the pixel
         * @param y: y-coordinate of the pixel
         * 
         * @return: Pixel at (x, y) represented as a vector (RGB)
        */
        float get_pixel(const int x, const int y) const {
            // Eigen uses (row, col) indicies
            return ImageUtil::get_pixel(this->image, x, y);
        }

        /**
         * @param x: x-coordiante of the pixel
         * @param y: y-coordiante of the pixel 
         * @param c: colour to set
        */
        void set_pixel(const int x, const int y, const float c) {
            // Eigen matrices (row, col) indicies
            ImageUtil::set_pixel(this->image, x, y, c);
        }

        /**
         * Converts the gray-scale image to a single-channel OpenCV matrix.
         * 
         * @return: Single-channel OpenCV matrix of the gray image. This matrix must be freed
        */
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
        std::tuple<Vector2f, float> compute_gradient(const int x, const int y, const HorizontalSobelKernel *sobel_x, const VerticalSobelKernel *sobel_y);

        /**
         * @param sobel_x: Horiozntal sobel kernel
         * @param sobel_y: Vertical sobel kernel
         * 
         * @return: RGB image containing the normal vectors of every pixel in the gray image
        */
        RGBImage *compute_normals(const HorizontalSobelKernel *sobel_x, const VerticalSobelKernel *sobel_y);
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
        Vector3f convolve(const int x, const int y, const GaussianKernel *kernel);

    public:
        RGBImage() : width(0), height(0) {}

        /**
         * Constructor for RGBImage.
         * 
         * Create a RGB image from the input RGB matrix.
         * 
         * @param width: width of the image
         * @param height: height of the image
         * @param image: image matrix to create the image from 
        */
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
        RGBImage(const int width, const int height, const Vector3f colour);

        /**
         * Constructor for RGB.
         * 
         * Create a RGB image from the OpenCV matrix
         * 
         * @param width: width of the image
         * @param height: height of the image
         * @param cv_image: OpenCV matrix to create the image from
        */
        RGBImage(const int width, const int height, const cv::Mat cv_image);

        ~RGBImage() {
            delete image;
        }

        int get_width() const {
            return this->width;
        }

        int get_height() const {
            return this->height;
        }

        RGBMatrix *get_image() const {
            return this->image;
        }

        /**
         * @param x: x-coordinate of the pixel
         * @param y: y-coordinate of the pixel
         * 
         * @return: Pixel at (x, y) represented as a vector (RGB)
        */
        Vector3f get_pixel(const int x, const int y) const {
            // Eigen uses (row, col) indicies
            return ImageUtil::get_pixel(this->image, x, y);
        }

        /**
         * @param x: x-coordiante of the pixel
         * @param y: y-coordiante of the pixel 
         * @param c: colour to set
        */
        void set_pixel(const int x, const int y, const Vector3f c) {
            // Eigen matrices (row, col) indicies
            ImageUtil::set_pixel(this->image, x, y, c);
        }

        /**
         * Converts the RGB image to a three-channel OpenCV matrix.
         * 
         * @return: Three-channel OpenCV matrix of the RGB image. This matrix must be freed.
        */
        cv::Mat *to_cv_mat();

        /**
         * Performs blurring using colvolution with the provided Gaussian kernel
         * 
         * @param kernel: Gaussian kernel 
         * 
         * @return: Blurred image. This image must be freed.
        */
        RGBImage *gaussian_blur(const GaussianKernel *kernel);

        /**
         * @return: The average pixel colour of the image
        */
        Vector3f average_colour();

        /**
         * Computes the difference between the image and the compare_image
         * 
         * @return: GrayImage containing the difference between each pixel. This image must be freed
        */
        GrayImage* difference(const RGBImage *compare_image);

        /**
         * Computes the luminosity image of the RGB image
         * 
         * @return GrayImage containing the luminosity of every pixel. This image must be freed.
        */
        GrayImage *luminosity();
};

