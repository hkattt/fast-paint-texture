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
|   ├── image.hpp
|   ├── kernel.hpp
|   ├── paint.hpp
├── out
│   ├── paint-cherry-blossom.png
│   ├── paint-cool-cat.png
│   ├── paint-nature.jpg
│   └── paint-nature.png
├── scripts
│   ├── clean.sh
│   ├── make-run.sh
│   ├── make.sh
│   └── run.sh
└── src
    ├── image.cpp
    ├── kernal.cpp
    ├── main.cpp
    └── paint.cpp
```

Use the following scripts in the *root directory* for simple usage.
* `scripts/make.sh`: Builds the project
* `scripts/run.sh`: Runs the program on all the images in `imgs`
* `scripts/make-run.sh`: Builds and runs the project
* `scripts/clean.sh`: Removes the created build files