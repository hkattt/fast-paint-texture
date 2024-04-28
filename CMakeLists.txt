cmake_minimum_required(VERSION 3.10)
project(FastPaintTexture)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

# Warn about unused code
add_compile_options(-Wunused)

# include_directories(/usr/local/include ./include)
add_executable(fast-paint-texture main.cpp rasterizer.hpp rasterizer.cpp)
target_link_libraries(fast-paint-texture ${OpenCV_LIBRARIES})