cmake_minimum_required(VERSION 3.16)
project(netcom)

set(CMAKE_CXX_STANDARD 11)
include_directories(header/)
add_library(netcom source/crc.cpp)
