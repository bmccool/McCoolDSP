cmake_minimum_required(VERSION 3.1)
project(McCoolDSP CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(McCoolDSP McCoolDSP.cpp)
