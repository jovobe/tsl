cmake_minimum_required(VERSION 3.12)
project(tsl_tests VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# application
set(TSL_TESTS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${TSL_INCLUDE_DIR})
include_directories(${TSL_TESTS_INCLUDE_DIR})
add_subdirectory(src)
