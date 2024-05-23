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

        RGBImage *source_image;

        // Arrays used for stroke drawing 
        int *counters = nullptr;
        Eigen::Vector3f *old_colours = nullptr;
        float *total_mask = nullptr;

        int cur_counter = 0;
        Eigen::Vector3f cur_colour; 
        
        /**
         * TODO: Update
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
        void paint_layer(RGBImage *ref_image, RGBImage *canvas, GrayImage *height_map, int radius);

        float compose_height(float h);

        void render_stroke(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, AntiAliasedCircle *mask);

        void render_stroke_point(RGBImage *canvas, GrayImage *height_map, int x, int y, AntiAliasedCircle *mask);

        void render_stroke_line(RGBImage *canvas, GrayImage *height_map, int x1, int y1, int x2, int y2, AntiAliasedCircle *mask);

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
            delete[] counters;
            delete[] old_colours;
            delete[] total_mask;
        }

        RGBImage *get_source_image() {
            return this->source_image;
        }

        std::tuple<RGBImage*, GrayImage*> paint();
};
