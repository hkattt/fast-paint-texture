#include <opencv2/opencv.hpp>

#include "paint.hpp"
#include "image.hpp"
#include "kernel.hpp"
#include "stroke.hpp"
#include "parameters.hpp"

using namespace std;

/** PUBLIC METHODS */

Paint::Paint(int width, int height, cv::Mat source_image) {
    // Ensure dimensions are valid
    if (source_image.cols != width || source_image.rows != height) {
        throw std::invalid_argument("Unable to create rasterizer: input image dimensions \
            do not match specified width and height.");
    }
    
    this->width = width;
    this->height = height;
    this->frame_buf.resize(width * height);
    this->depth_buf.resize(width * height);
    this->source_image = Image(width, height, source_image);
}

Image Paint::paint() {
    int brushes[ProgramParameters::num_layers];
    Image canvas, ref_image;
    GaussianKernel kernel;
    float sigma;
    int kernel_len;

    // Create the painting canvas
    canvas = Image(width, height, this->source_image.average_colour());

    // Create brushes (from largest to smallest)
    brushes[ProgramParameters::num_layers - 1] = ProgramParameters::min_brush_size;
    for (int i = ProgramParameters::num_layers - 2; i >= 0; i--) {
        brushes[i] = 2 * brushes[i + 1];
    }

    for (int brush_radius : brushes) {
        std::cout << "Painting layer with brush radius: " << brush_radius << std::endl;

        // Standard deviation used for Gaussian blur
        sigma = ProgramParameters::blur_factor * brush_radius;
        // Length of the Gaussian kernel window
        kernel_len = std::max(8 * sigma, 3.0f);

        kernel = GaussianKernel(kernel_len, sigma);

        // Apply Gaussian blur
        ref_image = this->source_image.gaussian_blur(&kernel);

        // Paint a layer
        this->paint_layer(&ref_image, &canvas, brush_radius);
    }
    return canvas;
}

void Paint::paint_layer(Image *ref_image, Image *canvas, int radius) {
    std::vector<Stroke> strokes;
    Stroke stroke;
    cv::Mat differences;
    int grid, max_x, max_y, diff_x, diff_y;
    float area_error, max_diff, current_diff;

    // Compute the difference between the reference image and the canvas
    differences = ref_image->difference(canvas);

    grid = ProgramParameters::grid_fac * radius;

    for (int y = 0; y < this->width; y += grid) {
        for (int x = 0; x < this->width; x+= grid) {
            // Reset area error, maximum difference, and maximum difference coordiantes
            area_error = 0.0f;
            max_x = 0, max_y = 0;
            max_diff = 0.0f;

            // Iterate over differences surrounding the current point
            for (int j = -(grid / 2); j < (grid / 2); j++) {
                for (int i = -(grid / 2); i < (grid / 2); i++) {
                    diff_x = x + i;
                    diff_y = y + j;

                    // Checks if coordinates are valid
                    if (diff_x < 0 || diff_x > this->width || diff_y < 0 || diff_y > this->height) {
                        continue;
                    }

                    current_diff = differences.at<float>(diff_y, diff_x);

                    // Sum the error ear (x, y)
                    area_error += current_diff;

                    // Check if the current difference is greater than the maximum difference
                    if (current_diff > max_diff) {
                        max_x = x;
                        max_y = y;
                        max_diff = current_diff;
                    }
                }
            }
            area_error /= grid * grid;

            if (area_error > ProgramParameters::threshold) {
                stroke = Stroke(max_x, max_y, radius, ref_image, canvas);
                strokes.push_back(stroke);
            }
        }
    }
    // Shuffle the strokes
    std::random_shuffle(strokes.begin(), strokes.end());

    // Render the strokes to the canvas
    for (Stroke stroke : strokes) {
        stroke.render_to_image(canvas);
    }
}

cv::Mat Paint::to_image() {
    cv::Mat image(height, width, CV_32FC3, frame_buf.data());
    image.convertTo(image, CV_8UC3, 1.0f);
    return image;
}

/** PRIVATE METHODS */

int Paint::get_index(int x, int y) {
    return (y * width) + x;
}

void Paint::set_pixel(const Vector2i &point, const Eigen::Vector3f &color) {
    int ind = get_index(point.x(), point.y());
    frame_buf[ind] = color;
}
