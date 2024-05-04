# Fast Paint Texture Research Project

## Overview
Implementing and extending (hopefully) Aaron Hertzmann's [fast paint texture algorithm](https://www.dgp.toronto.edu/papers/ahertzmann_NPAR2002.pdf). 

## Usage
The program has the following usage
```
./fast-paint-texture (input image) (output image)
```

## Scripts
Before trying to run any of these scripts, create the `build` directory in the project root. The expected project hierarchy is shown below.

```
├── CMakeLists.txt
├── README.md
├── build
├── imgs
│   ├── cherry-blossom.png
│   ├── cool-cat.png
│   ├── nature.jpg
│   └── nature.png
├── include
│   ├── image.hpp
│   ├── kernal.hpp
│   ├── paint.cpp
│   ├── parameters.hpp
│   └── stroke.cpp
├── out
│   ├── paint-cherry-blossom.png
│   ├── paint-cool-cat.png
│   ├── paint-nature.jpg
│   └── paint-nature.png
├── scripts
│   ├── clean.sh
│   ├── make.sh
│   ├── make-run.sh
│   └── run.sh
└── src
    ├── image.cpp
    ├── kernal.cpp
    ├── main.cpp
    ├── paint.cpp
    └── stroke.cpp
```

Use the following scripts in the *root directory* for simple usage.
* `scripts/make.sh`: Builds the project
* `scripts/run.sh image.png`: Runs the program on `imgs/image.png` and saves the output as `out/paint-image.png`
* `scripts/make-run.sh image.png`: Builds and runs the program on `imgs/image.png` and saves the output as `out/paint-image.png`
* `scripts/clean.sh`: Removes the created build files