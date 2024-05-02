#pragma once

#include <Eigen/Eigen>

#include "image.hpp"

using namespace Eigen;

/**
 * The main painting class responsible for implementing the fast-paint-texture algorithm
*/
class Paint {
    private:
        // Dimensions of the image
        int width, height;

        Image source_image;

        std::vector<Eigen::Vector3f> frame_buf;
        std::vector<float> depth_buf;

        /** ---------- Image Processing Parameters ---------- */

        int min_brush_size = 2;     // Smallest brush radius
        float blur_factor = 2;      // Radius of blurring in reference image to brush radius
        
        int get_index(int x, int y);
        
        void set_pixel(const Vector2i& point, const Eigen::Vector3f& color);

    public:
        /**
         * Constructor for Paint.
         * 
         * @param width: The width of the rasterizer window.
         * @param height: The height of the rasterizer window.
         * @param source_image: The input image to be painted.
        */
        Paint(int width, int height, cv::Mat source_image);

        /**
         * Performs the fast-paint-texture algorithm on the source image
        */
        cv::Mat paint();

        std::vector<Eigen::Vector3f>& frame_buffer() {
            return frame_buf;
        }

        std::vector<float>& depth_buffer() {
            return depth_buf;
        }

        /**
         * Converts the frame buffer to an OpenCV image.
         * 
         * @return cv::Mat The OpenCV image
        */
        cv::Mat to_image();
};
