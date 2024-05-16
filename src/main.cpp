#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <Eigen/Dense>

#include "paint.hpp"

using namespace std;
using namespace Eigen;

int main(int argc, const char **argv) {
    // Name of the input and output image files
    std::string input_file;
    std::string output_file;
    // Path to the input and output image directories
    std::string input_path = "../imgs/";
    std::string output_path = "../out/";

    // No arguments provided
    if (argc < 3) {
        std::cout << "Usage: fast-paint-texture (input file) (output file)"
                  << std::endl;
        return 1;
    } 
    // Input file and output file provided
    else if (argc == 3) {
        input_file = argv[1];
        output_file = argv[2];
    }
    // More than two arguments provided
    else {
        std::cout << "Too many arguments provided"
                  << std::endl;
        return 1;
    }

    // Load input image with colour
    cv::Mat input_image =  cv::imread(input_path + input_file, cv::IMREAD_COLOR);
    if (input_image.empty()) {
        std::cerr << "Error: Could not open the input image" << std::endl;
        return -1;
    }

    cout << "Loaded: " << input_file <<  " (" << input_image.cols << "x" << input_image.rows << ")" << ::endl;

    // Create a Paint instance for the input image
    Paint paint(input_image.cols, input_image.rows, input_image);

    // Paint the image and save the output file
    Image *output_image = paint.paint();
    cv::imwrite(output_path + output_file, output_image->get_image());

    cout << "Result saved to: " << output_path + output_file << std::endl;

    // Free memory
    delete output_image;

    return 0;
}