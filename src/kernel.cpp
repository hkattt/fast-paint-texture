#include <math.h>

#include "kernel.hpp"

using namespace std;

Kernel::Kernel(int len, float sigma) {
    float value, sum;

    this->len = len;
    this->cached_values.resize(len * len);

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
            this->cached_values[j * len + i] = value;

            sum += value;
        }
    }
    // Normalise the cached values - the values should sum to 1
    for (int j = 0; j < len; j++) {
        for (int i = 0; i < len; i++) {
            this->cached_values[j * len + i] /= sum;
        }
    }
}