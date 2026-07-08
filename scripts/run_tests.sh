#!/bin/bash

mkdir -p build 
cd build
rm -rf ./*
cmake ..
make -j"$(getconf _NPROCESSORS_ONLN)"
cd ..

rm -f examples/*.ll
for file in examples/*.c; do 
    base=$(basename "$file" .c); 
    echo "---------- Compiling $file ----------"; 
    build/c_compiler $file; 
done