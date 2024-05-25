#include "shader.hpp"

Eigen::Vector3f BlinnPhongShader::shade(Eigen::Vector3f colour, Eigen::Vector3f pos, Eigen::Vector3f light_pos, Eigen::Vector3f view_pos, Eigen::Vector3f normal) {
    Eigen::Vector3f ka = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
    Eigen::Vector3f kd = colour / 255.0f;
    Eigen::Vector3f ks = Eigen::Vector3f(2.5f, 2.5f, 2.5f);

    Eigen::Vector3f amb_intensity = Eigen::Vector3f(100.0f, 100.0f, 100.0f);
    Eigen::Vector3f intensity = Eigen::Vector3f(100.0f, 100.0f, 100.0f);

    Eigen::Vector3f output_colour = Eigen::Vector3f::Zero();

    // Apply ambient lighting
    output_colour += ka.cwiseProduct(amb_intensity);

    Eigen::Vector3f v, l, h;

    // View direction
    v = (view_pos - pos).normalized();

    // Light direction 
    l = (light_pos - pos).normalized();

    // Half-vector 
    h = (v + l).normalized();

    // Diffuse lighting. Fall-off not considered
    float N_dot_l = std::max(0.0f, normal.dot(h));
    output_colour += kd.cwiseProduct(intensity) * N_dot_l;

    // Specular lighting. Fall-off not considered
    float N_dot_h = std::max(0.0f, normal.dot(h));
    float p = 150.0f;
    output_colour += ks.cwiseProduct(intensity) * std::pow(N_dot_h, p);

    return output_colour;
}