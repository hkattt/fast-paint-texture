#pragma once 

#include <Eigen/Eigen>

#include "light.hpp"

using namespace Eigen;

class Shader {
    public:
        virtual Eigen::Vector3f shade(Eigen::Vector3f colour, Eigen::Vector3f pos, std::vector<Light> lights, Eigen::Vector3f view_pos, Eigen::Vector3f normal) = 0;

        virtual ~Shader() {}
};

class BlinnPhongShader : public Shader {
    private:
        // Specular exponent
        float p = 25.0f;
        
    public:
        BlinnPhongShader() = default;

        Eigen::Vector3f shade(Eigen::Vector3f colour, Eigen::Vector3f pos, std::vector<Light> lights, Eigen::Vector3f view_pos, Eigen::Vector3f normal);
};