#pragma once

#include <vector>

/**
 * Gaussian kernel object class
*/
class Kernel {
    private:
        // Length of the kernel window (i.e. it is a len by len grid)
        int len;
        // Computed values of the Gaussian Kernel
        std::vector<float> cached_values;

    public:
        Kernel(int len, float sigma);

        int get_len() const {
            return this->len;
        }

        /**
         * @param x: x index into the kernel window
         * @param y: y index into the kernel window
         * 
         * @return kernel value at (x, y)
        */
        float get_value(int x, int y) const {
            if (x < 0 || x > len || y < 0 || y > len) {
                throw std::invalid_argument(
                    "Invalid argument: (" + std::to_string(x) + ", " + std::to_string(y) + ") for kernel with length: " + std::to_string(len)
                );
            }
            return this->cached_values[x * len + y];
        }
};