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
    // Name of the input file, texture file (final output), paint file (output),and the height file (output)
    std::string input_file, texture_file, paint_file, height_file;
    // Path to the input and output image directories
    std::string input_path = "../imgs/";
    std::string stroke_texture_path = "../stroke-textures/";
    std::string texture_path = "../texture/";
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
    texture_file = "texture-" + input_file;
    height_file = "height-" + input_file;

    // Load input image with colour
    cv::Mat input_image =  cv::imread(input_path + input_file, cv::IMREAD_COLOR);
    if (input_image.empty()) {
        std::cerr << "Error: Could not open the input image" << std::endl;
        return -1;
    }

    cout << "Loaded: " << input_file <<  " (" << input_image.cols << "x" << input_image.rows << ")" << ::endl;

    // Load brush stroke textures
    cv::Mat height_texture_image = cv::imread(stroke_texture_path + "height.png", cv::IMREAD_GRAYSCALE);
    if (height_texture_image.empty()) {
        std::cerr << "Error: Could not open the height texture" << std::endl;
        return -1;
    }
    Texture *height_texture = new Texture(height_texture_image.cols, height_texture_image.rows, height_texture_image);

    cv::Mat opacity_texture_image = cv::imread(stroke_texture_path + "opacity.png", cv::IMREAD_GRAYSCALE);
    if (opacity_texture_image.empty()) {
        std::cerr << "Error: Could not open the opacity texture" << std::endl;
        return -1;
    }
    Texture *opacity_texture = new Texture(opacity_texture_image.cols, opacity_texture_image.rows, opacity_texture_image);

    #ifdef ANIMATE
        std::cout << "Running in animation mode" << std::endl;
    #endif

    // Create a fast-paint-texture instance for the input image
    FastPaintTexture paint(input_image.cols, input_image.rows, input_image, height_texture, opacity_texture);

    // Shader to apply to the image
    BlinnPhongShader blinn_phong_shader = BlinnPhongShader();
    LambertianShader lambertian_shader = LambertianShader();
    OrenNayarShader oren_nayar_shader = OrenNayarShader();
    ToonShader toon_shader = ToonShader();
    NormalShader normal_shader = NormalShader();

    // Apply the fast-paint-texture to the input image
    RGBImage *texture_image, *paint_image;
    GrayImage *height_map;
    std::tie<RGBImage*, RGBImage*, GrayImage*>(texture_image, paint_image, height_map) = paint.fast_paint_texture(&toon_shader);

    // Save the shaded image
    cv::Mat *cv_texture_image = texture_image->to_cv_mat();
    cv::imwrite(texture_path + texture_file, *cv_texture_image);
    cout << "Texture image saved to: " << texture_path + texture_file << std::endl;

    // Save the painted image
    cv::Mat *cv_paint_image = paint_image->to_cv_mat();
    cv::imwrite(paint_path + paint_file, *cv_paint_image);
    cout << "Image saved to: " << paint_path + paint_file << std::endl;

    // Save the height map
    cv::Mat *cv_height_map = height_map->to_cv_mat();
    cv::imwrite(height_path + height_file, *cv_height_map);
    cout << "Height map saved to: " << height_path + height_file << std::endl;

    // Free memory
    delete texture_image;
    delete cv_texture_image;

    delete paint_image;
    delete cv_paint_image;
    
    delete height_map;
    delete cv_height_map;

    if (height_texture != nullptr) delete height_texture;
    if (opacity_texture != nullptr) delete opacity_texture;

    return 0;
}