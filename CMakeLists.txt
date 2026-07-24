cmake_minimum_required(VERSION 3.22.1)

project(c_compiler)


set(CMAKE_CXX_STANDARD 23)
file(GLOB frontendSources ./frontend/*.cpp)
file(GLOB utils ./utils/*.cpp)

add_compile_options(-Wall -Wpedantic)
add_library(frontend STATIC ${frontendSources} SymbolTable.cpp)
target_compile_options(frontend PRIVATE -Wno-psabi)
add_library(utils STATIC ${utils})
target_compile_options(utils PRIVATE -Wno-psabi)
add_executable(c_compiler main.cpp Compiler.cpp SymbolTable.cpp)
target_compile_options(c_compiler PRIVATE -Wno-psabi)

target_link_libraries(c_compiler PUBLIC frontend)
target_link_libraries(c_compiler PUBLIC utils)
