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

        Image *source_image;
       
        std::vector<Eigen::Vector3f> frame_buf;
        std::vector<float> depth_buf;
       
        int get_index(int x, int y);
        
        void set_pixel(const Vector2i& point, const Eigen::Vector3f& color);

        /**
         * Paints a layer onto the canvas.
         * 
         * Implements the paintLayer psuedo-code from Painterly Rendering with Curved Brush 
         * Strokes of Multiple Sizes by Aaron Hertzmann.
         * 
         * @param ref_image: Reference (target) image
         * @param canvas: Canvas to paint the image onto
         * @param height_map: Height map of the image
         * @param radius: Radius of the brush stroke
        */
        void paint_layer(Image *ref_image, Image *canvas, int radius);

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
         * Destructor for Paint.
        */
        ~Paint() {
            delete source_image;
        }

        /**
         * Paints the source image onto the canvas.
         * 
         * Implements the paint psuedo-code from Painterly Rendering with Curved Brush 
         * Strokes of Multiple Sizes by Aaron Hertzmann.
         * 
         * @return: Painted image. This image must be freed
        */
        Image* paint();

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
