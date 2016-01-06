cmake_minimum_required(VERSION 3.3)
project(go)

set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${dir}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${dir}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${dir}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp cube.cpp cube.h cube_turn.cpp base.h crc64.cpp cube_parser.cpp command.cpp command.h)
add_executable(go ${SOURCE_FILES})