#pragma once

#include <opencv2/opencv.hpp>
#include <Eigen/Eigen>

#include "image.hpp"

using namespace Eigen;

class Stroke {
    private:
        float radius;
        Eigen::Vector3f colour;
        std::vector<Eigen::Vector2i> control_points;

    public:
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
        Stroke(int x, int y, float radius, Image ref_image, Image canvas);
};