cmake_minimum_required(VERSION 3.22.1)

project(c_compiler)


set(CMAKE_CXX_STANDARD 17)
file(GLOB frontendSources ./frontend/*.cpp)
file(GLOB backendSources ./backend/*.cpp)

add_compile_options(-Wall)
add_library(frontend STATIC ${frontendSources} symtab_utils.cpp)
add_library(backend STATIC ${backendSources} symtab_utils.cpp)
add_executable(c_compiler main.cpp compiler.cpp symtab_utils.cpp )


target_link_libraries(c_compiler PUBLIC frontend)
target_link_libraries(c_compiler PUBLIC backend)

set_property(TARGET c_compiler PROPERTY CXX_STANDARD 17)

include(CTest)
add_test(NAME "Parsing Expressions test"
		 COMMAND python3 expr_test.py
		 WORKING_DIRECTORY  ../tests/expr_test)