cmake_minimum_required(VERSION 3.22.1)

project(sael_compiler)


set(CMAKE_CXX_STANDARD 17)
file(GLOB frontendSources ./frontend/*.cpp)
file(GLOB backendSources ./backend/*.cpp)

add_library(frontend STATIC ${frontendSources} symtab_utils.cpp)
add_executable(sael-compiler main.cpp
	compiler.cpp symtab_utils.cpp ${backendSources})


target_link_libraries(sael-compiler PUBLIC frontend)


set_property(TARGET sael-compiler PROPERTY CXX_STANDARD 17)

include(CTest)
add_test(NAME "Parsing Expressions test"
		 COMMAND python3 expr_test.py
		 WORKING_DIRECTORY  ../tests/expr_test)