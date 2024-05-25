#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <Eigen/Dense>

#include "paint.hpp"
#include "shader.hpp"

using namespace std;
using namespace Eigen;

int main(int argc, const char **argv) {
    // Name of the input file, shade file (final output), paint file (output),and the height file (output)
    std::string input_file, shade_file, paint_file, height_file;
    // Path to the input and output image directories
    std::string input_path = "../imgs/";
    std::string paint_path = "../paint/";
    std::string height_path = "../height/";

    // No arguments provided
    if (argc < 2) {
        std::cout << "Usage: fast-paint-texture (input file)" << std::endl;
        return 1;
    } 
    // Input file provided
    else if (argc == 2) {
        input_file = argv[1];
    }
    // More than one arguments provided
    else {
        std::cout << "Too many arguments provided" << std::endl;
        return 1;
    }
    paint_file = "paint-" + input_file;
    shade_file = "shade-" + input_file;
    height_file = "height-" + input_file;

    // Load input image with colour
    cv::Mat input_image =  cv::imread(input_path + input_file, cv::IMREAD_COLOR);
    if (input_image.empty()) {
        std::cerr << "Error: Could not open the input image" << std::endl;
        return -1;
    }

    cout << "Loaded: " << input_file <<  " (" << input_image.cols << "x" << input_image.rows << ")" << ::endl;

    // Create a Paint instance for the input image
    Paint paint(input_image.cols, input_image.rows, input_image);

    // Paint the input image
    RGBImage *output_image;
    GrayImage *output_height_map;
    std::tie<RGBImage*, GrayImage*>(output_image, output_height_map) = paint.paint();

    // Shade the output image
    BlinnPhongShader shader = BlinnPhongShader();
    Eigen::Vector3f view_pos = Eigen::Vector3f(input_image.cols / 2, input_image.rows / 2, 5);
    Eigen::Vector3f light_pos = Eigen::Vector3f(input_image.cols / 2, input_image.rows / 2, 5);
    RGBImage *shaded_image = paint.apply_lighting(output_image, output_height_map, &shader, view_pos, light_pos);

    // Save the shaded image
    cv::Mat *cv_output_shaded_image = shaded_image->to_cv_mat();
    cv::imwrite(paint_path + shade_file, *cv_output_shaded_image);
    cout << "Shaded image saved to: " << paint_path + shade_file << std::endl;

    // Save the painted image
    cv::Mat *cv_output_image = output_image->to_cv_mat();
    cv::imwrite(paint_path + paint_file, *cv_output_image);
    cout << "Image saved to: " << paint_path + paint_file << std::endl;

    // Save the height map
    cv::Mat *cv_output_height_map = output_height_map->to_cv_mat();
    cv::imwrite(height_path + height_file, *cv_output_height_map);
    cout << "Height map saved to: " << height_path + height_file << std::endl;

    // Free memory
    delete output_image;
    delete output_height_map;

    delete cv_output_image;
    delete cv_output_height_map;
    
    delete shaded_image;
    delete cv_output_shaded_image;

    return 0;
}