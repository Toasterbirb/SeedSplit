#!/bin/sh
rm -rf build
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cp -r ../res ./
