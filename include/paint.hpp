#pragma once

#include <Eigen/Eigen>

#include "image.hpp"
#include "stroke.hpp"
#include "shader.hpp"
#include "light.hpp"

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
        Vector3f *old_colours = nullptr;
        float *total_mask = nullptr;

        int cur_counter = 0;
        Vector3f cur_colour; 

        // Textures
        Texture *height_texture;
        Texture *opacity_texture;
        
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

        float compose_height(float stroke_height, float stroke_opacity, float current_height);

        void render_stroke(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, AntiAliasedCircle *mask);

        void render_stroke_point(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, int x, int y, AntiAliasedCircle *mask);

        void render_stroke_line(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, int x1, int y1, int x2, int y2, AntiAliasedCircle *mask);

    public:
        /**
         * TODO: Update?
         * Constructor for Paint.
         * 
         * @param width: The width of the rasterizer window.
         * @param height: The height of the rasterizer window.
         * @param source_image: The input image to be painted.
        */
        Paint(int width, int height, cv::Mat source_image, Texture *height_texture, Texture *opacity_texture);

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

        RGBImage *apply_lighting(RGBImage *image, GrayImage *height_map, Shader *shader, Vector3f view_pos, std::vector<Light> lights);
};
