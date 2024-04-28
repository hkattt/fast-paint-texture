# Rebuild the project
cd build
cmake ..
make

# Run on all the images
./fast-paint-texture cherry-blossom.png paint-cherry-blossom.png
./fast-paint-texture cool-cat.png paint-cool-cat.png
./fast-paint-texture nature.png paint-nature.png