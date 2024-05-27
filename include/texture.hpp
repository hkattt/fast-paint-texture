#pragma once

#include "image.hpp"

class Texture {
    private:
        GrayImage *texture;

    public:
        Texture() {}
        
        Texture(int width, int height, cv::Mat cv_image) {
            this->texture = new GrayImage(width, height, cv_image);
        }

        ~Texture() {
            delete this->texture;
        }

        int get_width() {
            return this->texture->get_width();
        }

        int get_height() {
            return this->texture->get_height();
        }

        float get_texture_value(Eigen::Vector2f uv_coords);
};   