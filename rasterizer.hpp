#pragma once

#include <Eigen/Eigen>

using namespace Eigen;

namespace rst {
    enum class Buffers { Color = 1, Depth = 2 };

    inline Buffers operator|(Buffers a, Buffers b) {
        return Buffers((int)a | (int)b);
    }

    inline Buffers operator&(Buffers a, Buffers b) {
        return Buffers((int)a & (int)b);
    }

    /*
    * For the curious : The draw function takes two buffer id's as its arguments. These two
    * structs make sure that if you mix up with their orders, the compiler won't compile
    * it. Aka : Type safety
    * */
    struct pos_buf_id {
        int pos_id = 0;
    };

    struct ind_buf_id {
        int ind_id = 0;
    };

    struct col_buf_id {
        int col_id = 0;
    };

    class rasterizer {
        private:
            int width, height;
            std::vector<Eigen::Vector3f> frame_buf;
            std::vector<float> depth_buf;

            int get_index(int x, int y);
            void set_pixel(const Vector2i& point, const Eigen::Vector3f& color);

        public:
            /**
             * Constructor for rasterizer.
             * 
             * @param w: The width of the rasterizer window.
             * @param h: The height of the rasterizer window.
             * @param input_image: The input image to be loaded into the rasterizer.
            */
            rasterizer(int w, int h, cv::Mat input_image);

            std::vector<Eigen::Vector3f>& frame_buffer() {
                return frame_buf;
            }
            std::vector<float>& depth_buffer() {
                return depth_buf;
            }

            /**
             * Converts the frame buffer to an OpenCV image.
             * 
             * @return cv::Mat The OpenCV image
            */
            cv::Mat to_image();
    };
}  // namespace rst
