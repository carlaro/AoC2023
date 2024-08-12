#!/bin/bash

rm -r build
mkdir build
cmake -B build
cd build
make
for executable in ./*.out; do 
    $executable& 
done;
cd ..
