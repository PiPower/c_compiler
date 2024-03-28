cmake_minimum_required(VERSION 3.22.1)

project(LizardAutoDiff)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
endif()

set(CMAKE_CXX_STANDARD 17)
file(GLOB frontendSources ./frontend/*.cpp)

add_executable(c_compiler	main.cpp
							compiler.cpp
							${frontendSources})