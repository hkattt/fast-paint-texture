#include "stroke.hpp"
#include "parameters.hpp"
#include "debug.hpp"

Stroke::Stroke(int x0, int y0, float radius, RGBImage *ref_image, RGBImage *canvas) {
    Eigen::Vector2f d, g, last;
    Eigen::Vector3f ref_pixel, canvas_pixel, new_pixel;
    float grad_mag;

    // Set the colour and radius of the stroke
    this->colour = ref_image->get_pixel(x0, y0);
    this->radius = radius;

    float x = x0, y = y0;

    // Add first control point
    this->control_points.push_back(Vector2f(x, y));

    // Sobel kernels used to compute image gradient
    HorizontalSobelKernel sobel_x = HorizontalSobelKernel::get_instance();
    VerticalSobelKernel sobel_y = VerticalSobelKernel::get_instance();

    d = Vector2f::Zero();
    last = Vector2f::Zero();

    // Distance between control points
    int length = radius * ProgramParameters::length_fac;

    for (int i = 1; i < ProgramParameters::max_stroke_length; i++) {
        ref_pixel = ref_image->get_pixel(x, y);
        canvas_pixel = canvas->get_pixel(x, y);
        
        // Get the unit vector of gradient (gx, gy) and gradient magnitutde
        std::tie<Eigen::Vector2f, float>(g, grad_mag) = ref_image->compute_gradient(x, y, &sobel_x, &sobel_y);
        
        // Gradient is too small
        if (length * grad_mag < 1) {
            return;
        }

        // Compute normal direction
        // (dx, dy) = (-gy, gx)
        d[0] = -g.y();
        d[1] = g.x();

        // Reverse direction if necessary
        if (i > 1 && last.x() * d.x() + last.y() * d.y() < 0) {
            // (dx, dy) = (-dx, -dy)
            d[0] = -d.x();
            d[1] = -d.y();
        }

        // Filter stroke direction
        d = ProgramParameters::filter_fac * d + (1 - ProgramParameters::filter_fac) * last;
        d.normalize();

        last = d;

        x = x + length * d.x();
        y = y + length * d.y();

        // Ensure the control point is valid
        if (x < 0 || x >= ref_image->get_width() || y < 0 || y >= ref_image->get_height()) {
            return;
        }

        new_pixel = ref_image->get_pixel(x, y);

        if (i >= ProgramParameters::min_stroke_length && 
            (ref_pixel - canvas_pixel).norm() < (this->colour - new_pixel).norm()) {
            return;
        }

        // Add the new control point
        this->control_points.push_back(Eigen::Vector2f(x, y));
    }   
}

bool Stroke::limit_is_done() {
    if (this->limit.size() < 3) {
        return true;
    }

    // Difference between the second and first points in the limit 
    Eigen::Vector2f d = this->limit[1] - this->limit[0];

    // Angle between the second and first point in the limit
    float last_theta = std::atan2(d.y(), d.x());

    float new_theta;
    float t_diff;

    for (int i = 2; i < this->limit.size(); i++) {
        // Difference between the current and previous point in the limit
        d = limit[i] - limit[i - 1];

        // Angle between the current and previous point in the limit
        new_theta = std::atan2(d.y(), d.x());

        // Difference between the current and last angle (in radians)
        t_diff = std::fmod(std::abs(new_theta - last_theta), M_PI);

        // Ensures that the distance between the points and the angle between the points
        // is small enough
        if ((d.x() >= this->neighbourhood || d.y() >= this->neighbourhood) && t_diff > this->theta_tol) {
            return false;
        }

        last_theta = new_theta;
    }

    // Clean out extra points from the limit curve

    std::vector<Eigen::Vector2f> pts;
    
    pts.push_back(this->limit[0]);

    Eigen::Vector2f last_pt = this->limit[0];

    Eigen::Vector2f current_pt, next_pt;

    for (int i = 1; i < this->limit.size() - 1; i++) {
        current_pt = this->limit[i];
        next_pt = this->limit[i + 1];

        last_theta = std::atan2(current_pt.y() - last_pt.y(), current_pt.x() - last_pt.x());
        new_theta = std::atan2(next_pt.y() - current_pt.y(), next_pt.x() - current_pt.x());

        t_diff = std::fmod(std::abs(new_theta - last_theta), M_PI);

        if (t_diff > this->theta_tol) {
            pts.push_back(current_pt);

            last_pt = current_pt;
        }
    }

    if (!clean || pts.size() == this->limit.size() - 1) {
        return true;
    }

    pts.push_back(this->limit[this->limit.size() - 1]);

    // Free old limit vector
    this->limit.clear();
    this->limit.shrink_to_fit();

    this->limit = pts;
    return true;
}

void Stroke::compute_limit_curve() {
    std::vector<Eigen::Vector2f> split;
    int split_len;

    this->limit = this->control_points;

    // Continue interpolating points until the limit is complete
    while (!this->limit_is_done()) {
        split_len = 2 * this->limit.size() - 1;
        split.resize(split_len);

        for (int i = 0; i < split_len; i++) {
            if (i % 2 == 0) {
                split[i] = this->limit[i / 2];
            } 
            else {
                split[i] = 0.5 * this->limit[i / 2] + this->limit[(i / 2) + 1];
            }
        }

        this->limit.resize(split_len);

        this->limit[0] = split[0];
        this->limit[split_len - 1] = split[split_len - 1];
        
        // Interpolate the values in split
        for (int i = 1; i < split_len - 1; i++) {
            this->limit[i] = 0.25 * split[i - 1] + 0.5 * split[i] + 0.25 * split[i + 1];
        }
    }
}