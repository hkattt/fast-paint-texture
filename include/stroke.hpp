#pragma once

#include <Eigen/Eigen>

#include "texture.hpp"

using namespace Eigen;

class Stroke {
    private:
        int radius;
        Vector3f colour;
        std::vector<Vector2f> control_points;
        std::vector<Vector2f> limit;

        // Angle threshold - used to ensure smooth interpolation
        const double theta_tol = 0.1;
        // Distance threshold - used to ensure smooth interpolation
        const int neighbourhood = 2;
        const bool clean = false;

        Texture *height_texture = nullptr;
        Texture *opacity_texture = nullptr;

        // Bottom-left and top-right coordinates for bounding box
        Vector2i bottom_left;
        Vector2i top_right;

        /**
         * Determines if the limit has been completed.
         * Removes redundant points from the limit if needed.
        */
        bool limit_is_done();

        /**
         * Performs cubic b-spline interpolation on the control points
         * The interpolated points are stored in the limit vector
        */
        void compute_limit_curve();

    public:
        Stroke() {}

        /**
         * Constructor for Stroke. 
         * 
         * Creates a stroke from a given point to make the canvas better
         * match the reference image. Implements the makeSplineStroke psuedo-code
         * from Painterly Rendering with Curved Brush Strokes of Multiple Sizes
         * by Aaron Hertzmann.
         * 
         * @param x: x-coordinate
         * @param y: y-coordinate
         * @param radius: radius of the stroke
         * @param ref_image: reference image 
         * @param canvas: canvas image - where the stroke will be drawn
         * @param luminosity: luminosity of the reference image. Used to compute image gradients
        */
        Stroke(int x, int y, int radius, RGBImage *ref_image, RGBImage *canvas, GrayImage *luminosity);

        /**
         * @return: The limit of the Stroke. Computes the limit if needed
        */
        std::vector<Vector2f> get_limit() {
            if (this->limit.size() == 0) {
                compute_limit_curve();
            }
            return this->limit;
        }

        Vector3f get_colour() {
            return this->colour;
        }

        Vector2f get_uv_coords(int x, int y);

        float get_height(int x, int y);

        float get_opacity(int x, int y);

        void set_height_texture(Texture *height_texture) {
            this->height_texture = height_texture;
        }

        void set_opacity_texture(Texture *opacity_texture) {
            this->opacity_texture = opacity_texture;
        }

        void compute_bounding_box(int width, int height);
};