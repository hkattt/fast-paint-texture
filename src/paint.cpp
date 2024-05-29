#include <opencv2/opencv.hpp>

#include "paint.hpp"
#include "image.hpp"
#include "shader.hpp"
#include "kernel.hpp"
#include "stroke.hpp"
#include "parameters.hpp"

using namespace std;

FastPaintTexture::FastPaintTexture(int width, int height, cv::Mat source_image, Texture *height_texture, Texture *opacity_texture) {
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
    this->old_colours = new Vector3f[width * height];

    this->height_texture = height_texture;
    this->opacity_texture = opacity_texture;
}

std::tuple<RGBImage*, RGBImage*, GrayImage*> FastPaintTexture::fast_paint_texture(Shader *shader) {
    RGBImage *paint_image, *texture_image; 
    GrayImage *height_map;

    Light light1 = Light(Vector3f(this->width / 4, this->height / 4, 500), Vector3f(1.0f, 1.0f, 1.0f));
    Light light2 = Light(Vector3f(this->width / 4, 3 * this->height / 4, 500), Vector3f(1.0f, 1.0f, 1.0f));
    Light light3 = Light(Vector3f(3 * this->width / 4, this->height / 4, 500), Vector3f(1.0f, 1.0f, 1.0f));
    Light light4 = Light(Vector3f(3 * this->width / 4, 3 * this->height / 4, 500), Vector3f(1.0f, 1.0f, 1.0f));
    std::vector<Light> lights = {light1, light2, light3, light4};

    Vector3f view_pos = Vector3f(this->width / 2, this->height / 2, 1000);

    std::tie<RGBImage*, GrayImage*>(paint_image, height_map) = this->paint();

    texture_image = this->texture(paint_image, height_map, shader, view_pos, lights);

    return std::tuple<RGBImage*, RGBImage*, GrayImage*>(texture_image, paint_image, height_map);
}

std::tuple<RGBImage*, GrayImage*> FastPaintTexture::paint() {
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

RGBImage *FastPaintTexture::texture(RGBImage *image, GrayImage *height_map, Shader *shader, Vector3f view_pos, std::vector<Light> lights) {
    // Sobel kernels used to compute image gradient
    HorizontalSobelKernel sobel_x = HorizontalSobelKernel::get_instance();
    VerticalSobelKernel sobel_y = VerticalSobelKernel::get_instance();
    RGBImage *normals = height_map->compute_normals(&sobel_x, &sobel_y);

    Vector3f pos, new_colour, pixel;

    RGBMatrix *shaded_image = new RGBMatrix(this->height, this->width);

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            pos = Vector3f(x, y, 0);
            pixel = image->get_pixel(x, y);
            new_colour = shader->shade(pixel, pos, lights, view_pos, normals->get_pixel(x, y));
            ImageUtil::set_pixel(shaded_image, x, y, new_colour);
        }
    }

    // Free memory
    delete normals;

    return new RGBImage(this->width, this->height, shaded_image);
}

void FastPaintTexture::paint_layer(RGBImage *ref_image, RGBImage *canvas, GrayImage *height_map, int radius) {
    std::vector<Stroke> strokes;
    GrayImage *differences, *luminosity;
    int grid, max_x, max_y;
    float area_error, max_diff, current_diff;

    // Compute the difference between the reference image and the canvas
    differences = ref_image->difference(canvas);

    // Compute the luminosity of the reference image. Used to compute image gradients
    luminosity = ref_image->luminosity();

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
                Stroke stroke = Stroke(max_x, max_y, radius, ref_image, canvas, luminosity);

                stroke.compute_bounding_box(canvas->get_width(), canvas->get_height());

                stroke.set_height_texture(this->height_texture);
                stroke.set_opacity_texture(this->opacity_texture);
                
                strokes.push_back(stroke);
            }
        }
    }
    // Free memory
    delete differences;
    delete luminosity;

    // Render the strokes to the canvas
    for (Stroke stroke : strokes) {
        this->render_stroke(canvas, height_map, &stroke, &brush);

        #ifdef ANIMATE
            cv::Mat *cv_canvas = canvas->to_cv_mat();
            cv::imshow("canvas", *cv_canvas);
            cv::waitKey(1);
            delete cv_canvas;
        #endif
    }    
}

// TODO: Move this into the GrayImage class?
float FastPaintTexture::compose_height(float stroke_height, float stroke_opacity, float current_height) {
    float height_blend = ImageUtil::alpha_blend(stroke_height, current_height, stroke_opacity / 255);

    return height_blend + 0.001f * this->cur_counter;
}

void FastPaintTexture::render_stroke(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, AntiAliasedCircle *mask) {
    std::vector<Vector2f> limit = stroke->get_limit();

    this->cur_counter++; // TODO: Does this need to be global
    this->cur_colour = stroke->get_colour(); // TODO: Is this really needed if we are passing color

    if (limit.size() == 1) {
        this->render_stroke_point(canvas, height_map, stroke, limit[0].x(), limit[0].y(), mask);
        return;
    }

    for (int i = 0; i < limit.size() - 1; i++) {
        this->render_stroke_line(canvas, height_map, stroke, limit[i].x(), limit[i].y(), limit[i + 1].x(), limit[i + 1].y(), mask);
    }
}

void FastPaintTexture::render_stroke_point(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, int x, int y, AntiAliasedCircle *mask) {
    int new_x, new_y, ind;
    float alpha, composed_height;
    Vector3f blended_colour;


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

                composed_height = this->compose_height(stroke->get_height(new_x, new_y), stroke->get_opacity(new_x, new_y), height_map->get_pixel(new_x, new_y));
                height_map->set_pixel(new_x, new_y, composed_height);
            } 
            else {
                if (this->total_mask[ind] < alpha) {
                    this->total_mask[ind] = alpha;
                    
                    blended_colour = ImageUtil::alpha_blend(this->cur_colour, this->old_colours[ind], alpha);
                    canvas->set_pixel(new_x, new_y, blended_colour);

                    composed_height = this->compose_height(stroke->get_height(new_x, new_y), stroke->get_opacity(new_x, new_y), height_map->get_pixel(new_x, new_y));
                    height_map->set_pixel(new_x, new_y, composed_height);
                }
            }
        }
    }
}

void FastPaintTexture::render_stroke_line(RGBImage *canvas, GrayImage *height_map, Stroke *stroke, int x1, int y1, int x2, int y2, AntiAliasedCircle *mask) {
    int xa, xb, ya, yb;
    float m, y;

    if (x1 == x2) {
        ya = std::min(y1, y2);
        yb = std::max(y1, y2);

        for (int y = ya; y < yb; y++) {
            this->render_stroke_point(canvas, height_map, stroke, x1, y, mask);
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
            this->render_stroke_point(canvas, height_map, stroke, x, y, mask);
            y += m;
        }
    }
}
