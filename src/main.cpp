#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <Eigen/Dense>

#include "paint.hpp"
#include "shader.hpp"
#include "light.hpp"

using namespace std;
using namespace Eigen;

int main(int argc, const char **argv) {
    // Name of the input file, shade file (final output), paint file (output),and the height file (output)
    std::string input_file, shade_file, paint_file, height_file;
    // Path to the input and output image directories
    std::string input_path = "../imgs/";
    std::string texture_path = "../textures/";
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

    // Load brush stroke textures
    cv::Mat height_texture_image = cv::imread(texture_path + "height.png", cv::IMREAD_GRAYSCALE);
    if (height_texture_image.empty()) {
        std::cerr << "Error: Could not open the height texture" << std::endl;
        return -1;
    }
    Texture *height_texture = new Texture(height_texture_image.cols, height_texture_image.rows, height_texture_image);
    //Texture *height_texture = nullptr;

    cv::Mat opacity_texture_image = cv::imread(texture_path + "opacity.png", cv::IMREAD_GRAYSCALE);
    if (opacity_texture_image.empty()) {
        std::cerr << "Error: Could not open the opacity texture" << std::endl;
        return -1;
    }
    Texture *opacity_texture = new Texture(opacity_texture_image.cols, opacity_texture_image.rows, opacity_texture_image);

    #ifdef ANIMATE
        std::cout << "Running in animation mode" << std::endl;
    #endif

    // Create a Paint instance for the input image
    Paint paint(input_image.cols, input_image.rows, input_image, height_texture, opacity_texture);

    // Paint the input image
    RGBImage *output_image;
    GrayImage *output_height_map;
    std::tie<RGBImage*, GrayImage*>(output_image, output_height_map) = paint.paint();

    // Shade the output image
    BlinnPhongShader shader = BlinnPhongShader();
    Light light1 = Light(Vector3f(input_image.cols / 4, input_image.rows / 4, 500), Vector3f(1.0f, 1.0f, 1.0f));
    Light light2 = Light(Vector3f(input_image.cols / 4, 3 * input_image.rows / 4, 500), Vector3f(1.0f, 1.0f, 1.0f));
    Light light3 = Light(Vector3f(3 * input_image.cols / 4, input_image.rows / 4, 500), Vector3f(1.0f, 1.0f, 1.0f));
    Light light4 = Light(Vector3f(3 * input_image.cols / 4, 3 * input_image.rows / 4, 500), Vector3f(1.0f, 1.0f, 1.0f));
    std::vector<Light> lights = {light1, light2};
    Eigen::Vector3f view_pos = Eigen::Vector3f(input_image.cols / 2, input_image.rows / 2, 1000);
    RGBImage *shaded_image = paint.apply_lighting(output_image, output_height_map, &shader, view_pos, lights);

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

    #ifdef ANIMATE
        // view_pos = Eigen::Vector3f(input_image.cols / 2, input_image.rows / 2, 5);
        // for (int i = 0; i < input_image.cols; i++) {
        //     light_pos = Eigen::Vector3f(i, input_image.rows / 2, 500);
        //     shaded_image = paint.apply_lighting(output_image, output_height_map, &shader, view_pos, light_pos);
        //     cv_output_shaded_image = shaded_image->to_cv_mat();
            
        //     cv::imshow("shaded_image", *cv_output_shaded_image);

        //     cv::waitKey(1);

        //     delete shaded_image;
        //     delete cv_output_shaded_image;
        // }
    #endif

    // Free memory
    delete output_image;
    delete output_height_map;

    delete cv_output_image;
    delete cv_output_height_map;
    
    delete shaded_image;
    delete cv_output_shaded_image;

    if (height_texture != nullptr) delete height_texture;
    if (opacity_texture != nullptr) delete opacity_texture;

    return 0;
}