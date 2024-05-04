#pragma once

#include <vector>

/**
 * Anstract Kernel class
*/
class Kernel {
    protected:
        // Length of the kernel window (i.e. it is a len by len grid)
        int len;
        // Stores kernel values
        std::vector<float> values;

    public:
        /**
         * @return The length of the kernel window 
        */    
        int get_len() const {
            return this->len;
        }

        /**
         * @param x: x index into the kernel window
         * @param y: y index into the kernel window
         * 
         * @return kernel value at (x, y)
        */
        float get_value(int x, int y) const;
};

/**
 * Concrete Gaussian Kernel class
*/
class GaussianKernel : public Kernel {
    public:
        GaussianKernel() {}
        
        GaussianKernel(int len, float sigma);
};

/**
 * Abstract Sobel Kernel class
*/
class SobelKernel : public Kernel {};

/**
 * Concrete Horizontal Sobel Kernel
 * Implements the Singelton design pattern.
*/
class HorizontalSobelKernel : public SobelKernel {
    private:
        HorizontalSobelKernel();
        
    public:
        static HorizontalSobelKernel& get_instance() {
            static HorizontalSobelKernel instance;
            return instance;
        }
};

/**
 * Concrete Vertical Sobel Kernel
 * Implements the Singelton design pattern
*/
class VerticalSobelKernel : public SobelKernel {
    private:
        VerticalSobelKernel();

    public:
        static VerticalSobelKernel& get_instance() {
            static VerticalSobelKernel instance;
            return instance;
        }
};


