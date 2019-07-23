cmake_minimum_required(VERSION 3.10)
project(tsl_tests VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(ext/googletest EXCLUDE_FROM_ALL)

# application
set(TSL_TESTS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${TSL_INCLUDE_DIR})
include_directories(${TSL_TESTS_INCLUDE_DIR})
add_subdirectory(src)
