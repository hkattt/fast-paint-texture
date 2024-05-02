#include <opencv2/opencv.hpp>

#include "paint.hpp"
#include "image.hpp"
#include "kernel.hpp"

using namespace std;

/** PUBLIC METHODS */

Paint::Paint(int width, int height, cv::Mat source_image) {
    cv::Vec3b pixel;
    int ind;

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

    // Load the input image into the frame buffer
    for (int row = 0; row < source_image.rows; row++) {
        for (int col = 0; col < source_image.cols; col++) {
            // Extract the current pixel from the input image
            // Note that OpenCV indexes via (row, column)
            pixel = source_image.at<cv::Vec3b>(row, col);
            
            // Add the current pixel to the frame buffer
            // cv::Mat origin (0, 0) starts at the top-left corner
            ind = get_index(col, row);
            frame_buf[ind] = Eigen::Vector3f(pixel[0], pixel[1], pixel[2]);
        }
    }
}

cv::Mat Paint::paint() {
    // Standard deviation used for Gaussian blur
    float sigma = this->blur_factor * this->min_brush_size;
    // Length of the Gaussian kernel window
    int kernel_len = std::max(8 * sigma, 3.0f);

    Kernel kernel = Kernel(kernel_len, sigma);
    
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
