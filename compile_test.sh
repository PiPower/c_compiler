for file in examples/*.c; do 
    base=$(basename "$file" .c); 
    echo "Compiling $file"; 
    build/c_compiler $file; 
    clang examples/$base.ll; 
done