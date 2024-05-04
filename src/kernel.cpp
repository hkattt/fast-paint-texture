#include <math.h>
#include <stdexcept>

#include "kernel.hpp"

using namespace std;

GaussianKernel::GaussianKernel(int len, float sigma) {
    float value, sum;

    this->len = len;
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
    this->values = {
        1, 0, -1, 
        2, 0, -2, 
        1, 0, -1
    };
}

VerticalSobelKernel::VerticalSobelKernel() {
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
