#pragma once 

#include <Eigen/Eigen>

#include "light.hpp"

using namespace Eigen;

class Shader {
    public:
        virtual Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal) = 0;

        virtual ~Shader() {}
};

class BlinnPhongShader : public Shader {
    private:
        // Specular exponent
        float p = 25.0f;
        
    public:
        BlinnPhongShader() = default;

        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};

class LambertianShader : public Shader {
    public:
        LambertianShader() = default;

        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};

class OrenNayarShader : public Shader {
    private:
        // Roughness
        float sigma = 1.0;
        // Proportion of light that is diffusely reflected
        float albedo = 0.8;

    public:
        OrenNayarShader() = default;

        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};

class ToonShader : public Shader {
    public:
        ToonShader() = default;

        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};

class NormalShader : public Shader {
    public:
        NormalShader() = default;

        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};