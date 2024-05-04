#include <opencv2/opencv.hpp>

#include "paint.hpp"
#include "image.hpp"
#include "kernel.hpp"
#include "stroke.hpp"
#include "parameters.h"

using namespace std;

/** PUBLIC METHODS */

Paint::Paint(int width, int height, cv::Mat source_image) {
    // Ensure dimensions are valid
    if (source_image.cols != width || source_image.rows != height) {
        throw std::invalid_argument("Unable to create rasterizer: input image dimensions \
            do not match specified width and height.");
    }
    
    this->width = width;
    this->height = height;
    this->frame_buf.resize(width * height);
    this->depth_buf.resize(width * height);
    this->source_image = Image(width, height, source_image);
}

cv::Mat Paint::paint() {
    // Standard deviation used for Gaussian blur
    float sigma = ProgramParameters::blur_factor * ProgramParameters::min_brush_size;
    // Length of the Gaussian kernel window
    int kernel_len = std::max(8 * sigma, 3.0f);

    GaussianKernel kernel = GaussianKernel(kernel_len, sigma);
    
    return source_image.gaussian_blur(&kernel); 
}

cv::Mat Paint::to_image() {
    cv::Mat image(height, width, CV_32FC3, frame_buf.data());
    image.convertTo(image, CV_8UC3, 1.0f);
    return image;
}

/** PRIVATE METHODS */

int Paint::get_index(int x, int y) {
    return (y * width) + x;
}

void Paint::set_pixel(const Vector2i &point, const Eigen::Vector3f &color) {
    int ind = get_index(point.x(), point.y());
    frame_buf[ind] = color;
}
