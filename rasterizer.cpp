#include <opencv2/opencv.hpp>

#include "rasterizer.hpp"

using namespace std;

/** PUBLIC METHODS */

rst::rasterizer::rasterizer(int w, int h, cv::Mat input_image) : width(w), height(h) {
    cv::Vec3b pixel;
    int ind;

    // Ensure dimensions are valid
    if (input_image.cols != w || input_image.rows != h) {
        throw std::invalid_argument("Unable to create rasterizer: input image dimensions \
            do not match specified width and height.");
    }
    
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);

    // Load the input image into the frame buffer
    for (int row = 0; row < input_image.rows; row++) {
        for (int col = 0; col < input_image.cols; col++) {
            // Extract the current pixel from the input image
            // Note that OpenCV indexes via (row, column)
            pixel = input_image.at<cv::Vec3b>(row, col);
            
            // Add the current pixel to the frame buffer
            // cv::Mat origin (0, 0) starts at the top-left corner
            ind = get_index(col, row);
            frame_buf[ind] = Eigen::Vector3f(pixel[0], pixel[1], pixel[2]);
        }
    }
}

cv::Mat rst::rasterizer::to_image() {
    cv::Mat image(height, width, CV_32FC3, frame_buf.data());
    image.convertTo(image, CV_8UC3, 1.0f);
    return image;
}

/** PRIVATE METHODS */

int rst::rasterizer::get_index(int x, int y) {
    return (y * width) + x;
}

void rst::rasterizer::set_pixel(const Vector2i &point, const Eigen::Vector3f &color) {
    int ind = get_index(point.x(), point.y());
    frame_buf[ind] = color;
}
