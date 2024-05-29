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
class FastPaintTexture {
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

        /**
         * Renders a stroke onto the canvas and height map
         * 
         * @param canvas: Canvas to render the stroke onto 
         * @param height_map: Height map to render the stroke onto
         * @param stroke: Stroke to render 
         * @param mask: Anti-aliased circle kernel used to render the stroke
        */
        void render_stroke(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, AntiAliasedCircle *mask);

        /**
         * Renders a stroke point onto canvas and height map
         * 
         * @param canvas: Canvas to render the stroke onto 
         * @param height_map: Height map to render the stroke onto
         * @param stroke: Stroke to render 
         * @param x: x-coordinate of the point
         * @param y: y-coordinate of the point
         * @param mask: Anti-aliased circle kernel used to render the stroke point
        */
        void render_stroke_point(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, int x, int y, AntiAliasedCircle *mask);

        /**
         * Renders a stroke line onto the canvas and height map
         * 
         * @param canvas: Canvas to render the stroke onto 
         * @param height_map: Height map to render the stroke onto
         * @param stroke: Stroke to render 
         * @param x1: x-coordinate of the start point
         * @param y1: y-coordinate of the start point
         * @param x2: x-coordiante of the end point
         * @param y2: y-coordiante of the end point
         * @param mask: Anti-aliased circle kernel used to render the stroke
        */
        void render_stroke_line(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, int x1, int y1, int x2, int y2, AntiAliasedCircle *mask);
        
        /**
         * Paints an image onto the canvas and height map
         * 
         * Implements the paint psuedo-code from Painterly Rendering with Curved Brush Strokes of Multiple Sizes by Aaron Hertzmann. 
         * However, it also renders to a height map which is used to perform shading.
         * 
         * @return: Tuple containing the painted canvas and height map
        */
        std::tuple<RGBImage*, GrayImage*> paint();
        
        /**
         * Textures a painted image using its height map. 
         * 
         * @param image: Painted image to texture
         * @param height_map: Height map of the painted image
         * @param shader: Shader to use for lighting
         * @param view_pos: View/eye position
         * @param lights: Lights in the scene
         * 
         * @return: The textured canvas
        */
        RGBImage *texture(RGBImage *image, GrayImage *height_map, Shader *shader, Vector3f view_pos, std::vector<Light> lights);

    public:
        /**
         * Constructor for Paint.
         * 
         * @param width: The width of the rasterizer window.
         * @param height: The height of the rasterizer window.
         * @param source_image: The input image to be painted.
         * @param height_texture: Height texture for the brush strokes
         * @param opacity_texture: Opacity texture for the brush strokes
        */
        FastPaintTexture(int width, int height, cv::Mat source_image, Texture *height_texture, Texture *opacity_texture);

        /**
         * Destructor for Paint.
        */
        ~FastPaintTexture() {
            delete source_image;
            delete[] counters;
            delete[] old_colours;
            delete[] total_mask;
        }

        RGBImage *get_source_image() {
            return this->source_image;
        }

        /**
         * Implemented the Fast Paint Texture described by Aaron Hertzmann in Fast Paint Texture.
         * 
         * @return: Tuple containing the textured painted image, painted image, and height map.
        */
        std::tuple<RGBImage*, RGBImage*, GrayImage*> fast_paint_texture(Shader *shader);
};
