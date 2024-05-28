#pragma once 

#include <Eigen/Eigen>

#include "light.hpp"

using namespace Eigen;

class Shader {
    public:
        virtual Vector3f shade(Vector3f colour, Vector3f pos, std::vector<Light> lights, Vector3f view_pos, Vector3f normal) = 0;

        virtual ~Shader() {}
};

class BlinnPhongShader : public Shader {
    private:
        // Specular exponent
        float p = 25.0f;
        
    public:
        BlinnPhongShader() = default;

        Vector3f shade(Vector3f colour, Vector3f pos, std::vector<Light> lights, Vector3f view_pos, Vector3f normal);
};