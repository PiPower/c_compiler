#!/bin/bash
rm test_logs.txt
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
    echo "---------- Compiling $file ----------" >> test_logs.txt;  
    build/c_compiler $file  >> test_logs.txt
    echo >> test_logs.txt; 
done

rm .builtin_defines.hpp .c_search_paths.set