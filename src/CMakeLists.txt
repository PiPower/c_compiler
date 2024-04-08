cmake_minimum_required(VERSION 3.22.1)

project(c_compiler)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
endif()

set(CMAKE_CXX_STANDARD 17)
file(GLOB frontendSources ./frontend/*.cpp)
file(GLOB backendSources ./backend/*.cpp)


add_executable(c_compiler	main.cpp
							compiler.cpp
							${frontendSources}
							${backendSources})

include(CTest)

add_test(NAME "Scannet Test"
		 COMMAND python3 ./scannerTests/test_scanner.py
		 WORKING_DIRECTORY /home/mateusz/c++_projects/c_compiler/test)

add_test(NAME "Function call Test"
		 COMMAND python3 test_functioncall.py
		 WORKING_DIRECTORY /home/mateusz/c++_projects/c_compiler/test/callTest)