cmake_minimum_required(VERSION 3.22.1)

project(c_compiler)


set(CMAKE_CXX_STANDARD 23)
file(GLOB frontendSources ./frontend/*.cpp)
file(GLOB utils ./utils/*.cpp)

add_compile_options(-Wall -Wpedantic)
add_library(frontend STATIC ${frontendSources})
add_library(utils STATIC ${utils})
add_executable(c_compiler main.cpp Compiler.cpp)

target_link_libraries(c_compiler PUBLIC frontend)
target_link_libraries(c_compiler PUBLIC utils)

add_executable(lexer_test tests/test_lexer_2.cpp)
target_link_libraries(lexer_test PUBLIC frontend)
target_link_libraries(lexer_test PUBLIC utils)