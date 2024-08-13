#!/bin/bash

rm -r build
mkdir build
cmake -B build
cd build
make
./day1.out
./day2.out
./day3.out
./day4.out
./day5.out
./day6.out
./day7.out
./day8.out
./day9.out
./day10.out
./day11.out
cd ..
