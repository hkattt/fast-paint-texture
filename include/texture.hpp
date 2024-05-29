#pragma once

#include "image.hpp"

/** 
 * Gray-scale texture class.
*/
class Texture {
    private:
        // Texture image
        GrayImage *texture;

    public:
        Texture() {}
        
        /**
         * Texture constructor. 
         * 
         * @param width: width of the texture image
         * @param height: height of the texture image
         * @param cv_image: OpenCV matrix representing the image
        */
        Texture(int width, int height, cv::Mat cv_image) {
            this->texture = new GrayImage(width, height, cv_image);
        }

        ~Texture() {
            delete this->texture;
        }

        int get_width() const {
            return this->texture->get_width();
        }

        int get_height() const {
            return this->texture->get_height();
        }

        float get_texture_value(const Vector2f uv_coords) const;
};   