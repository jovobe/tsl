cmake_minimum_required(VERSION 3.10)
project(tsl VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# tinyobjloader
include_directories(ext/tinyobjloader)

# application
set(TSL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${TSL_INCLUDE_DIR})
add_subdirectory(src)
add_subdirectory(test)
