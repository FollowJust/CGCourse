cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(raytracing VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(thirdparty)
include_directories(src)
add_subdirectory(src)
