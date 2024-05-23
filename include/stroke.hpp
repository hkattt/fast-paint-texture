#pragma once

#include <opencv2/opencv.hpp>
#include <Eigen/Eigen>

#include "image.hpp"

using namespace Eigen;

class RGBImage;

class Stroke {
    private:
        float radius;
        Eigen::Vector3f colour;
        std::vector<Eigen::Vector2f> control_points;
        std::vector<Eigen::Vector2f> limit;

        // Angle threshold - used to ensure smooth interpolation
        const double theta_tol = 0.1;
        // Distance threshold - used to ensure smooth interpolation
        const int neighbourhood = 2;
        const bool clean = false;

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
         * @return: The limit of the Stroke. Computes the limit if needed
        */
        std::vector<Eigen::Vector2f> get_limit() {
            if (this->limit.size() == 0) {
                compute_limit_curve();
            }
            return this->limit;
        }

        Eigen::Vector3f get_colour() {
            return this->colour;
        }

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
        */
        Stroke(int x, int y, float radius, RGBImage *ref_image, RGBImage *canvas);
};