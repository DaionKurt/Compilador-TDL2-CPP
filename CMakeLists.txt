cmake_minimum_required(VERSION 3.6)
project(Generator_P5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(SOURCE_FILES main.cpp)
add_executable(Generator_P5 ${SOURCE_FILES})