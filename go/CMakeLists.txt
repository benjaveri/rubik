cmake_minimum_required(VERSION 3.3)
project(go)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp cube.cpp cube.h cube_turn.cpp base.h crc64.cpp cube_parser.cpp)
add_executable(go ${SOURCE_FILES})