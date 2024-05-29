#include "shader.hpp"
#include <iostream>
#include <math.h>

Vector3f BlinnPhongShader::shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal) {
    float ka = 0.1f;
    float kd = 0.6f;
    float ks = 0.3f;

    Vector3f amb_intensity = Vector3f(1.0f, 1.0f, 1.0f);

    // Scale the input colour between [0, 1]
    Vector3f scaled_colour = colour / 255.0f;

    Vector3f output_colour = Vector3f::Zero();

    // Apply ambient lighting
    output_colour += ka * amb_intensity;

    Vector3f v, l, h;

    // View direction
    v = (view_pos - pos).normalized();

    for (Light light : lights) {
        // Light direction 
        l = (light.get_position() - pos).normalized();

        // Half-vector 
        h = (v + l).normalized();

        // Diffuse lighting. Fall-off not considered
        float N_dot_l = std::max(0.0f, normal.dot(l));
        output_colour += kd * scaled_colour.cwiseProduct(light.get_intensity()) * N_dot_l;

        // Specular lighting. Fall-off not considered
        float N_dot_h = std::max(0.0f, normal.dot(h));
        output_colour += ks * light.get_intensity() * std::pow(N_dot_h, this->p);
    }
    output_colour /= lights.size();

    // Clamp the colour between [0, 1]
    output_colour.cwiseMin(1.0f).cwiseMax(0.0f);

    // Scale the colour from [0, 1] to [0, 255]
    return output_colour * 255.0f;
}

Vector3f LambertianShader::shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal) {
    Vector3f l, B_D;

    // Scale the input colour between [0, 1]
    Vector3f scaled_colour = colour / 255.0f;

    Vector3f output_colour = Vector3f::Zero();

    for (Light light : lights) {
        // Light direction
        l = (light.get_position() - pos).normalized();

        // Brightness of the diffusely reflect light
        B_D = std::max(normal.dot(l), 0.0f) * scaled_colour.cwiseProduct(light.get_intensity());

        output_colour += B_D;
    }
    output_colour /= lights.size();

    // Clamp the colour between [0, 1]
    output_colour.cwiseMin(1.0f).cwiseMax(0.0f);

    return output_colour * 255.0f;
}

/**
 * This implementation is based on: https://github.com/glslify/glsl-diffuse-oren-nayar/blob/master/index.glsl
*/
Vector3f OrenNayarShader::shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal) {
    // Scale the input colour between [0, 1]
    Vector3f scaled_colour = colour / 255.0f;

    Vector3f output_colour = Vector3f::Zero();

    float sigma_squared = this->sigma * this->sigma;

    float diffuse_factor, A, B, s, t, l_dot_v, n_dot_l, n_dot_v;
    
    Vector3f v, l;

    // View direction
    v = (view_pos - pos).normalized();

    for (Light light : lights) {
        // Light direction
        l = (light.get_position() - pos).normalized();

        l_dot_v = l.dot(v);
        n_dot_l = normal.dot(l);
        n_dot_v = normal.dot(v);

        s = l_dot_v - n_dot_l * n_dot_v;
        if (s < 0) {
            t = 1.0f;
        } else {
            t = std::max(n_dot_l, n_dot_v);
        }

        A = 1.0f + sigma_squared * (this->albedo / (sigma_squared + 0.13) + 0.5 / (sigma_squared + 0.33));
        B = 0.45f * sigma_squared / (sigma_squared + 0.09);

        diffuse_factor = this->albedo * std::max(0.0f, n_dot_l) * (A + B * s /t) / M_PI;

        output_colour += scaled_colour.cwiseProduct(light.get_intensity()) * diffuse_factor;
    }
    output_colour /= lights.size();

    // Clamp the colour between [0, 1]
    output_colour.cwiseMin(1.0f).cwiseMax(0.0f);

    return output_colour * 255.0f;
}

Vector3f ToonShader::shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal) {
    Vector3f l, B_D;

    // Scale the input colour between [0, 1]
    Vector3f scaled_colour = colour / 255.0f;

    Vector3f output_colour = Vector3f::Zero();

    float n_dot_l, c;

    for (Light light : lights) {
        // Light direction
        l = (light.get_position() - pos).normalized();

        n_dot_l = std::max(normal.dot(l), 0.0f);

        if (n_dot_l > 0.5) {
            c = 1.0f;
        } else if (n_dot_l > 0.2) {
            c = 0.7f;
        } else {
            c = 0.4f;
        }

        output_colour += c * scaled_colour.cwiseProduct(light.get_intensity());
    }
    output_colour /= lights.size();

    // Clamp the colour between [0, 1]
    output_colour.cwiseMin(1.0f).cwiseMax(0.0f);

    return output_colour * 255.0f;
}

Vector3f NormalShader::shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal) {
    // Convert normal vector from [-1, 1] range to [0, 1] range and then to [0, 255] rnage
    return 255.0f * ((normal.array() + 1.0f) / 2);
}