#include "shader.hpp"

Eigen::Vector3f BlinnPhongShader::shade(Eigen::Vector3f colour, Eigen::Vector3f pos, Eigen::Vector3f light_pos, Eigen::Vector3f view_pos, Eigen::Vector3f normal) {
    float ka = 0.1f;
    float kd = 0.6f;
    float ks = 0.8f;

    Eigen::Vector3f amb_intensity = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    Eigen::Vector3f intensity = Eigen::Vector3f(1.0f, 1.0f, 1.0f);

    colour /= 255.0f;

    Eigen::Vector3f output_colour = Eigen::Vector3f::Zero();

    // Apply ambient lighting
    output_colour += ka * amb_intensity;

    Eigen::Vector3f v, l, h;

    // View direction
    v = (view_pos - pos).normalized();

    // Light direction 
    l = (light_pos - pos).normalized();

    // Half-vector 
    h = (v + l).normalized();

    // Diffuse lighting. Fall-off not considered
    float N_dot_l = std::max(0.0f, normal.dot(l));
    output_colour += kd * colour.cwiseProduct(intensity) * N_dot_l;

    // Specular lighting. Fall-off not considered
    float N_dot_h = std::max(0.0f, normal.dot(h));
    output_colour += ks * intensity * std::pow(N_dot_h, this->p);

    // Clamp the colour between [0, 1]
    output_colour.cwiseMin(1.0f).cwiseMax(0.0f);

    // Scale the colour from [0, 1] to [0, 255]
    return output_colour * 255.0f;
}