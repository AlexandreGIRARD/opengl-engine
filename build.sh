#!/bin/sh

mkdir build
cd build
cmake ..
make
mv engine ../
cd ..

