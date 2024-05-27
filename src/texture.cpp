#include "texture.hpp"

float Texture::get_texture_value(Eigen::Vector2f uv_coords) {
    // Image coordinates
    float x = uv_coords.x() * (this->get_width() - 1);
    float y = uv_coords.y() * (this->get_height() - 1);

    // Convert to four integer coordinates
    int x0 = std::floor(x);
    int x1 = std::min(x0 + 1, this->get_width());
    int y0 = std::floor(y);
    int y1 = std::min(y0 + 1, this->get_height());

    float bot_left_val  = this->texture->get_pixel(x0, y0);
    float bot_right_val = this->texture->get_pixel(x1, y0);

    float top_left_val  = this->texture->get_pixel(x0, y1);
    float top_right_val = this->texture->get_pixel(x1, y1);

    // Determine the contribution of each pixel 
    float cont_x = x - x0;
    float cont_y = y - y0;

    // Combines values in the x-direction
    float combined_bot_value = (1 - cont_x) * bot_left_val + cont_x * bot_right_val;
    float combined_top_value = (1 - cont_x) * top_left_val + cont_x * top_right_val;

    // Combines values in the y-direction
    return (1 - cont_y) * combined_bot_value + cont_y * combined_top_value;
}