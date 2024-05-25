cmake_minimum_required(VERSION 3.10)
project(FastPaintTexture)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

# Set default value for ANIMATE option
option(ANIMATE "Enable animation" OFF)

# Check if ANIMATE option is enabled
if (ANIMATE)
    add_definitions(-DANIMATE)
endif()

include_directories(${EIGEN3_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS}, include)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Warn about unused code
add_compile_options(-Wunused)

add_executable(fast-paint-texture ${SOURCES})
target_link_libraries(fast-paint-texture ${OpenCV_LIBRARIES})