#!/bin/sh

cd install

mkdir build

cd build

cmake ..

make

sudo make install

cd ..

rm -rf build