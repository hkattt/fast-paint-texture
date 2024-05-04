#include "stroke.hpp"
#include "parameters.hpp"

Stroke::Stroke(int x, int y, float radius, Image *ref_image, Image *canvas) {
    Eigen::Vector2f d, g, last;
    Eigen::Vector3f ref_pixel, canvas_pixel;
    float grad_mag;

    // Set the colour and radius of the stroke
    this->colour = ref_image->get_pixel(x, y);
    this->radius = radius;

    // Add first control point
    this->control_points.push_back(Vector2i(x, y));

    // Sobel kernels used to compute image gradient
    HorizontalSobelKernel sobel_x = HorizontalSobelKernel::get_instance();
    VerticalSobelKernel sobel_y = VerticalSobelKernel::get_instance();

    d = Vector2f::Zero();
    last = Vector2f::Zero();

    for (int i = 0; i < ProgramParameters::max_stroke_length; i++) {
        ref_pixel = ref_image->get_pixel(x, y);
        canvas_pixel = canvas->get_pixel(x, y);

        if (i > ProgramParameters::min_stroke_length && (ref_pixel - canvas_pixel).norm() < (ref_pixel - this->colour).norm()) {
            return;
        }
        
        // Get the unit vector of gradient (gx, gy) and gradient magnitutde
        std::tie<Eigen::Vector2f, float>(g, grad_mag) = ref_image->compute_gradient(x, y, &sobel_x, &sobel_y);
        
        // Detect vanishing gradient
        // TODO: Use epsilon maybe?
        if (grad_mag == 0) {
            return;
        }

        // Compute normal direction
        // (dx, dy) = (-gy, gx)
        d[0] = -g.y();
        d[1] = g.x();

        // Reverse direction if necessary
        if (last.x() * d.x() + last.y() * d.y() < 0) {
            // (dx, dy) = (-dx, -dy)
            d[0] = -d.x();
            d[1] = -d.y();
        }

        // Filter stroke direction
        d = ProgramParameters::filter_fac * d + (1 - ProgramParameters::filter_fac) * last;
        d.normalize();

        x = x + radius * d.x();
        y = y + radius * d.y();

        last = d;

        // Add the new control point
        this->control_points.push_back(Eigen::Vector2i(x, y));
    }   
}

void Stroke::render_to_image(Image *canvas) {
    // TODO: Complete
    return;
}