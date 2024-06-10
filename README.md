# Fast Paint Texture Research Project

Final research project for COMP4610 Computer Graphics at the ANU. 

## Overview

Reimplemented Aaron Hertzmann's Fast Paint Texture algorithm presented in [Fast Paint Texture algorithm](https://www.dgp.toronto.edu/papers/ahertzmann_NPAR2002.pdf) in C++. The Fast Paint Texture algorithm extends Aaron Hertzmann's previous painting algorithm presented in [Painterly Rendering with Curved Brush Strokes of Multiple Sizes](https://dl.acm.org/doi/pdf/10.1145/280814.280951). Our project structure is based on [Aaron Hertzmann's Java implementation](https://github.com/hertzmann/painterJava/tree/main) of Painterly Rendering with Curved Brush Strokes of Multiple Sizes. 

Our primary contribution is applying different lighting shaders and brush stroke textures to the Fast Paint Texture algorith. 

## Usage
The program has the following usage
```
./fast-paint-texture (input-image) (shader)
```
Where
- `input-image` is the file name of the input image
- `shader` is the lighting shader to be used for rendering. `shader` can have the following values: `blinn-phong`, `lambertian`, `oren-nayar`, `toon`, and `normal`.

## Scripts
We provide `.sh` scripts for simple usage of the program. Before trying to run any of the scripts, create the `build` directory in the project root. The expected project hierarchy is shown below.

```
├── build
├── CMakeLists.txt
├── include
│   ├── image.hpp
│   ├── kernel.hpp
│   ├── light.hpp
│   ├── paint.hpp
│   ├── parameters.hpp
│   ├── shader.hpp
│   ├── stroke.hpp
│   └── texture.hpp
├── README.md
├── scripts
│   ├── clean.sh
│   ├── make-run.sh
│   ├── make.sh
│   └── run.sh
├── src
│   ├── image.cpp
│   ├── kernel.cpp
│   ├── main.cpp
│   ├── paint.cpp
│   ├── shader.cpp
│   ├── stroke.cpp
│   └── texture.cpp
├── stroke-textures
│   └── Brush stroke texture images
├── imgs
│   └── Input images
├── paint
│   └── Output painted images
├── height
│   └── Output painted height maps
└── texture
    └── Output textured painted images
```

Use the following scripts in the *root directory* for simple usage.
* `scripts/make.sh`: Builds the project
* `scripts/make.sh -ANIMATE`: Builds the project in animation mode. This will show how the canvas is painted. 
* `scripts/run.sh image.png shader`: Runs the program on `imgs/image.png` using the `shader` lighting shader and saves the output as `texture/shader-image.png`, `paint/paint-image.png` and `height/height-image.png`
* `scripts/make-run.sh image.png`: Builds and runs the program on `imgs/image.png` using the `shader` lighting shader and saves the output as `texture/shader-image.png`, `paint/paint-image.png` and `height/height-image.png`
* `scripts/clean.sh`: Deletes the current build files

**NOTE**: After building the project in animation mode, you must clean the project before it can be built in normal mode again. 
