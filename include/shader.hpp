#pragma once 

#include <Eigen/Eigen>

#include "light.hpp"

using namespace Eigen;

/**
 * Abstract light shader class.
*/
class Shader {
    public:
        /**
         * Shades a given pixel colour.
         * 
         * @param colour: Colour of the pixel being shaded
         * @param pos: Hit/colour posiiton
         * @param lights: Lights in the scene
         * @param view_pos: View/eye position
         * @param normal: Normal vector at the hit position
         * 
         * @return: Shaded pixel colour
        */
        virtual Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal) = 0;

        virtual ~Shader() {}
};

/**
 * Concrete Blinn-Phong shader class.
*/
class BlinnPhongShader : public Shader {
    private:
        // Specular exponent
        float p = 25.0f;
        
    public:
        BlinnPhongShader() = default;

        /**
         * See Shader::shade documentation
        */
        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};

/**
 * Concrete Lambertian shader class.
*/
class LambertianShader : public Shader {
    public:
        LambertianShader() = default;

        /**
         * See Shader::shade documentation
        */
        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};

/**
 * Concrete Oren-Nayar shader class.
*/
class OrenNayarShader : public Shader {
    private:
        // Roughness
        float sigma = 1.0;
        // Proportion of light that is diffusely reflected
        float albedo = 0.8;

    public:
        OrenNayarShader() = default;

        /**
         * See Shader::shade documentation
        */
        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};


/**
 * Concrete Toon shader (Cel shader) class.
*/
class ToonShader : public Shader {
    public:
        ToonShader() = default;

        /**
         * See Shader::shade documentation
        */
        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};

/**
 * Concrete Normal shader (Cel shader) class.
*/
class NormalShader : public Shader {
    public:
        NormalShader() = default;

        /**
         * See Shader::shade documentation
        */
        Vector3f shade(const Vector3f &colour, const Vector3f &pos, const std::vector<Light> &lights, const Vector3f &view_pos, const Vector3f &normal);
};