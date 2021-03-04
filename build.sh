#!/bin/sh

mkdir build
cd build
cmake ..
make -j4
mv engine ../
cd ..
