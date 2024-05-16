#include <math.h>
#include <stdexcept>

#include "kernel.hpp"

using namespace std;

AntiAliasedCircle::AntiAliasedCircle(int radius, float fall_off) {
    // Fall off cannot exceed the radius
    if (fall_off >= radius) {
        fall_off = radius - 1;
    }

    this->len = 2 * radius;
    this->centre_x = (this->len - 1) / 2;
    this->centre_y = this->centre_x;
    this->values.resize(len * len);

    // Effective radius of the circle
    float f = radius - fall_off;
    
    // Centre coordinates of the circle
    float cx = (this->len + 1) / 2.0f;
    float cy = (this->len + 1) / 2.0f;

    float dx, dy, d_squared, t;

    // Iterate over every pixel in the circle bounding box
    for (int x = 0; x < this->len; x++) {
        for (int y = 0; y < this->len; y++) {
            // Difference between the current coordinates and the centre of the circle
            dx = x - cx;
            dy = y - cy;

            // Distance (square) from the centre to the current pixel
            // Use distance square since sqrt is an expensive operation
            d_squared = dx * dx + dy * dy;

            // Current pixel is not in the circle
            if (d_squared > radius * radius) {
                this->values[y * len + x] = 0;
            } 
            else {
                // The current pixel is not in the effective radius of the circle
                if (d_squared > f * f) {
                    // Smooth pixel colour for anti-aliasing
                    t = (radius - std::sqrt(d_squared)) / fall_off;
                    this->values[y * len + x] = t * t * (3 - 2 * t);
                }
                // The current pixel is in the effective radius 
                else {
                    this->values[y * len + x] = 1;
                }
            }
        }
    }
}

GaussianKernel::GaussianKernel(int len, float sigma) {
    float value, sum;

    this->len = len;
    this->centre_x = (this->len - 1) / 2;
    this->centre_y = this->centre_x;
    this->values.resize(len * len);

    // Centre of the kernel window
    int centre = (len - 1) / 2;
    // Factor used to compute kernel value
    float factor = std::sqrt(2 * M_PI * std::pow(sigma, 2));

    // Iterates over every point in the kernel window
    sum = 0;
    for (int j = 0; j < len; j++) {
        for (int i = 0; i < len; i++) {
            // Compute Gaussian value at (i, j)
            // exp(-((i - centre)^2 + (j - centre)^2) / 2 * sigma^2) / sqrt(2 * pi * sigma^2)
            value =  std::exp(-(std::pow(i - centre, 2) + std::pow(j - centre, 2)) / (2 * std::pow(sigma, 2))) / factor;
            
            // Caches the value
            this->values[j * len + i] = value;

            sum += value;
        }
    }
    // Normalise the cached values - the values should sum to 1
    for (int j = 0; j < len; j++) {
        for (int i = 0; i < len; i++) {
            this->values[j * len + i] /= sum;
        }
    }
}

HorizontalSobelKernel::HorizontalSobelKernel() {
    this->len = 3;
    this->centre_x = 1;
    this->centre_y = 1;
    this->values = {
        1, 0, -1, 
        2, 0, -2, 
        1, 0, -1
    };
}

VerticalSobelKernel::VerticalSobelKernel() {
    this->len = 3;
    this->centre_x = 1;
    this->centre_y = 1;
    this->values = {
        1, 2, 1, 
        0, 0, 0, 
        -1, -2, -1
    };
}

float Kernel::get_value(int x, int y) const {
    if (x < 0 || x > len || y < 0 || y > len) {
        throw std::invalid_argument(
            "Invalid argument: (" + std::to_string(x) + ", " + std::to_string(y) + ") for kernel with length: " + std::to_string(len)
        );
    }
    return this->values[x * len + y];
}
