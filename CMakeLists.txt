cmake_minimum_required(VERSION 3.22.1)

project(sael_compiler)


set(CMAKE_CXX_STANDARD 17)
file(GLOB frontendSources ./frontend/*.cpp)
file(GLOB backendSources ./backend/*.cpp)


add_executable(sael-compiler	main.cpp
							compiler.cpp
							${frontendSources}
							${backendSources})

set_property(TARGET sael-compiler PROPERTY CXX_STANDARD 17)
