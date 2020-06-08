#!/bin/sh
GIT_DIR=$(git rev-parse --show-toplevel)
mkdir vulkan-headers && cd vulkan-headers
git clone https://github.com/KhronosGroup/Vulkan-Headers/ src
mkdir install
cd src && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$GIT_DIR/vulkan-headers ..
make install
cd ../..
rm -rf src
cd ..