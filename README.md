# Fast Paint Texture Research Project

## Overview
Implementing and extending (hopefully) Aaron Hertzmann's [fast paint texture algorithm](https://www.dgp.toronto.edu/papers/ahertzmann_NPAR2002.pdf). 

## Usage
The program has the following usage
```
./fast-paint-texture (input image)
```

## Scripts
Before trying to run any of these scripts, create the `build` directory in the project root. The expected project hierarchy is shown below.

```
├── CMakeLists.txt
├── height
│   ├── height-cherry-blossom.png
│   ├── height-cool-cat.png
│   ├── height-nature.png
│   ├── height-reis.png
│   └── height-star-wars.png
├── imgs
│   ├── cherry-blossom.png
│   ├── cool-cat.png
│   ├── nature.png
│   ├── reis.png
│   └── star-wars.png
├── include
│   ├── debug.hpp
│   ├── image.hpp
│   ├── kernel.hpp
│   ├── paint.hpp
│   ├── parameters.hpp
│   └── stroke.hpp
├── paint
│   ├── paint-cherry-blossom.png
│   ├── paint-cool-cat.png
│   ├── paint-nature.png
│   ├── paint-reis.png
│   └── paint-star-wars.png
├── README.md
├── scripts
│   ├── clean.sh
│   ├── make-run.sh
│   ├── make.sh
│   └── run.sh
└── src
    ├── image.cpp
    ├── kernel.cpp
    ├── main.cpp
    ├── paint.cpp
    └── stroke.cpp
```

Use the following scripts in the *root directory* for simple usage.
* `scripts/make.sh`: Builds the project
* `scripts/run.sh image.png`: Runs the program on `imgs/image.png` and saves the output as `paint/paint-image.png` and `height/height-image.png`
* `scripts/make-run.sh image.png`: Builds and runs the program on `imgs/image.png` and saves the output as `paint/paint-image.png` and `height/height-image.png`
* `scripts/clean.sh`: Removes the created build files