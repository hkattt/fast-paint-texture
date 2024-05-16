#pragma once

#include <vector>

/**
 * Anstract Kernel class
*/
class Kernel {
    protected:
        // Length of the kernel window (i.e. it is a len by len grid)
        int len;
        // Centre of the kernel 
        int centre_x, centre_y;
        // Stores kernel values
        std::vector<float> values;

    public:
        /**
         * @return The length of the kernel window 
        */    
        int get_len() const {
            return this->len;
        }

        int get_centre_x() const {
            return this->centre_x;
        }

        int get_centre_y() const {
            return this->centre_y;
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
 * Concrete Anti-aliased Circle Kernel class
*/
class AntiAliasedCircle : public Kernel {
    public:
        AntiAliasedCircle() {}

        AntiAliasedCircle(int radius, float fall_off);
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


