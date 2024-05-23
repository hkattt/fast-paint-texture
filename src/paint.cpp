#include <opencv2/opencv.hpp>

#include "paint.hpp"
#include "image.hpp"
#include "kernel.hpp"
#include "stroke.hpp"
#include "parameters.hpp"

using namespace std;

Paint::Paint(int width, int height, cv::Mat source_image) {
    // Ensure dimensions are valid
    if (source_image.cols != width || source_image.rows != height) {
        throw std::invalid_argument("Unable to create rasterizer: input image dimensions \
            do not match specified width and height.");
    }
    
    this->width = width;
    this->height = height;
    this->source_image = new RGBImage(width, height, source_image);

    this->counters = new int[width * height];
    this->total_mask = new float[width * height];
    this->old_colours = new Eigen::Vector3f[width * height];
}

std::tuple<RGBImage*, GrayImage*> Paint::paint() {
    int brushes[ProgramParameters::num_layers];
    RGBImage *canvas, *ref_image;
    GrayImage *height_map;
    GaussianKernel kernel;
    float sigma;
    int kernel_len;

    // Create the painting canvas
    canvas = new RGBImage(width, height, this->source_image->average_colour());
    height_map = new GrayImage(width, height, 0.0f);

    // Create brushes (from largest to smallest)
    brushes[ProgramParameters::num_layers - 1] = ProgramParameters::min_brush_size;
    for (int i = ProgramParameters::num_layers - 2; i >= 0; i--) {
        brushes[i] = 2 * brushes[i + 1];
    }

    // TODO: Move elsewhere?
    std::fill_n(this->counters, width * height, 0);
    std::fill_n(this->old_colours, width * height, canvas->get_pixel(0, 0));
    std::fill_n(this->total_mask, width * height, 0.0f);
    // TODO: Move elsewhere?
    this->cur_counter = 0;
    
    for (int brush_radius : brushes) {
        std::cout << "Painting layer with brush radius: " << brush_radius << std::endl;

        // Standard deviation used for Gaussian blur
        sigma = ProgramParameters::blur_factor * brush_radius;
        // Length of the Gaussian kernel window
        kernel_len = std::max(8 * sigma, 3.0f);

        kernel = GaussianKernel(kernel_len, sigma);

        // Apply Gaussian blur
        ref_image = this->source_image->gaussian_blur(&kernel);

        // Paint a layer
        this->paint_layer(ref_image, canvas, height_map, brush_radius);

        // Free memory
        delete ref_image;
    }
    return std::tuple<RGBImage*, GrayImage*>(canvas, height_map);
}

void Paint::paint_layer(RGBImage *ref_image, RGBImage *canvas, GrayImage *height_map, int radius) {
    std::vector<Stroke> strokes;
    GrayImage *differences;
    int grid, max_x, max_y;
    float area_error, max_diff, current_diff;

    // Compute the difference between the reference image and the canvas
    differences = ref_image->difference(canvas);

    // Brush mask
    AntiAliasedCircle brush = AntiAliasedCircle(radius, ProgramParameters::aa * radius);

    grid = std::max((int) ProgramParameters::grid_fac * radius, 1);

    for (int y = 0; y < this->height; y += grid) {
        for (int x = 0; x < this->width; x+= grid) {
            // Reset area error, maximum difference, and maximum difference coordiantes
            area_error = 0.0f;
            max_x = x, max_y = y;
            max_diff = 0.0f;

            // Iterate over differences surrounding the current point
            for (int j = y - (grid / 2); j <= y + (grid / 2); j++) {
                for (int i = x -(grid / 2); i <= x + (grid / 2); i++) {
                    // Checks if coordinates are valid
                    if (i < 0 || i >= this->width || j < 0 || j >= this->height) {
                        continue;
                    }

                    current_diff = differences->get_pixel(i, j); // TODO: Fix index

                    // Sum the error ear (x, y)
                    area_error += current_diff;

                    // Check if the current difference is greater than the maximum difference
                    if (current_diff > max_diff) {
                        max_x = i;
                        max_y = j;
                        max_diff = current_diff;
                    }
                }
            }
            // It is cheaper to check this than dividing area_error by grid * grid
            if (area_error > ProgramParameters::threshold * grid * grid) {
                Stroke stroke = Stroke(max_x, max_y, radius, ref_image, canvas);
                strokes.push_back(stroke);
            }
        }
    }
    // Shuffle the strokes
    // TODO: Figure out why we can't shuffle the strokes??
    //std::random_shuffle(strokes.begin(), strokes.end());

    // Render the strokes to the canvas
    for (Stroke stroke : strokes) {
        this->render_stroke(canvas, height_map, &stroke, &brush);
    }

    // Free memory
    delete differences;
}

// TODO: Move this into the GrayImage class?
float Paint::compose_height(float h) {
    float stroke_height = 1.0f; // TODO: Make this not constant
    float alpha = 0.5f; // TODO: Get this from somewhere else

    float height_blend = ImageUtil::alpha_blend(stroke_height, h, alpha);
    return height_blend + 0.001f * this->cur_counter;
}

void Paint::render_stroke(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, AntiAliasedCircle *mask) {
    std::vector<Eigen::Vector2f> limit = stroke->get_limit();

    this->cur_counter++;
    this->cur_colour = stroke->get_colour();

    if (limit.size() == 1) {
        this->render_stroke_point(canvas, height_map, limit[0].x(), limit[0].y(), mask);
        return;
    }

    for (int i = 0; i < limit.size() - 1; i++) {
        this->render_stroke_line(canvas, height_map, limit[i].x(), limit[i].y(), limit[i + 1].x(), limit[i + 1].y(), mask);
    }
}

void Paint::render_stroke_point(RGBImage *canvas, GrayImage *height_map, int x, int y, AntiAliasedCircle *mask) {
    int new_x, new_y, ind;
    float alpha, composed_height;
    Eigen::Vector3f blended_colour;


    for (int j = 0; j < mask->get_len(); j++) {
        for (int i = 0; i < mask->get_len(); i++) {
            new_x = x + i - mask->get_centre_x();
            new_y = y + j - mask->get_centre_y();

            // Checks if the coordinates are valid
            if (new_x < 0 || new_x >= this->width || new_y < 0 || new_y >= this->height) {
                continue;
            }

            alpha = mask->get_value(i, j);

            ind = new_y * this->width + new_x;

            if (this->counters[ind] < this->cur_counter) {
                this->counters[ind] = this->cur_counter;
                this->old_colours[ind] = canvas->get_pixel(new_x, new_y);
                this->total_mask[ind] = alpha;
                
                blended_colour = ImageUtil::alpha_blend(this->cur_colour, this->old_colours[ind], alpha);
                canvas->set_pixel(new_x, new_y, blended_colour);

                composed_height = this->compose_height(height_map->get_pixel(new_x, new_y));
                height_map->set_pixel(new_x, new_y, composed_height);
            } 
            else {
                if (this->total_mask[ind] < alpha) {
                    this->total_mask[ind] = alpha;
                    
                    blended_colour = ImageUtil::alpha_blend(this->cur_colour, this->old_colours[ind], alpha);
                    canvas->set_pixel(new_x, new_y, blended_colour);

                    composed_height = this->compose_height(height_map->get_pixel(new_x, new_y));
                    height_map->set_pixel(new_x, new_y, composed_height);
                }
            }
        }
    }
}

void Paint::render_stroke_line(RGBImage *canvas, GrayImage *height_map, int x1, int y1, int x2, int y2, AntiAliasedCircle *mask) {
    int xa, xb, ya, yb;
    float m, y;

    if (x1 == x2) {
        ya = std::min(y1, y2);
        yb = std::max(y1, y2);

        for (int y = ya; y < yb; y++) {
            this->render_stroke_point(canvas, height_map, x1, y, mask);
        }
    } 
    else {
        if (x1 < x2) {
            xa = x1;
            xb = x2;
            ya = y1;
            yb = y2;
        }
        else {
            xa = x2;
            xb = x1;
            ya = y2;
            yb = y1;
        }
        m = ((float) yb - ya) / (xb - xa);
        y = ya;

        for (int x = xa; x <= xb; x++) {
            this->render_stroke_point(canvas, height_map, x, y, mask);
            y += m;
        }
    }
}
