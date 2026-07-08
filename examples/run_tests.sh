#!/bin/bash
rm .builtin_defines.hpp .c_search_paths.set
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

rm .builtin_defines.hpp .c_search_paths.set