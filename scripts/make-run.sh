# Rebuild the project
cd build
cmake ..
make

# Run on input image and shader
./fast-paint-texture $1 $2