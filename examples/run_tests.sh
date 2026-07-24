#!/bin/bash
rm -f -r logs;
mkdir logs
rm -f .builtin_defines.hpp .c_search_paths.set
mkdir -p build_cnt
cd build_cnt
rm -rf ./*
cmake ..
make -j"$(getconf _NPROCESSORS_ONLN)"
cd ..

rm -f examples/*.ll
for file in examples/*.c; do 
    base=$(basename "$file" .c); 
    echo "---------- Compiling $file ----------"; 
    echo "---------- Compiling $file ----------">> logs/compiler_logs.txt 
    build_cnt/c_compiler $file | sed $'s/^/\t/' >> logs/compiler_logs.txt ;
    #pass for llvm correctness check
    echo "---------- Checking $file ----------" >>  logs/grammar_logs.txt;  
    opt -passes=verify examples/$base.ll -disable-output 2>&1 | sed $'s/^/\t/' >>  logs/grammar_logs.txt ;
    echo >> logs/grammar_logs.txt;
    #comparison check
    echo "---------- Comparing $base.ll ----------" &>> logs/diff_logs.txt;  
    diff examples/$base.ll  examples/verified/$base.ll 2>&1 | sed $'s/^/\t/' >> logs/diff_logs.txt;  
    echo >> logs/diff_logs.txt;
done

rm .builtin_defines.hpp .c_search_paths.set