#pragma once 

#include <Eigen/Eigen>

using namespace Eigen;

/**
 * Light class representing a point light
*/
class Light {
    private:
        Vector3f position;
        Vector3f intensity;

    public:
        Light(Vector3f position, Vector3f intensity) {
            this->position = position;
            this->intensity = intensity;
        }

        inline Vector3f get_position() const {
            return this->position;
        }

        inline Vector3f get_intensity() const {
            return this->intensity;
        }
};