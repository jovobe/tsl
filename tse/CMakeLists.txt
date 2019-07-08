cmake_minimum_required(VERSION 3.10)
project(tse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# deps
find_package(OpenGL REQUIRED)

# imgui
add_subdirectory(ext/imgui)
include_directories(ext/imgui)

# pfd
include_directories(ext/pfd)

# tsl
add_subdirectory(../tsl bin)
include_directories(../tsl/include)

# application
set(TSE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${TSE_INCLUDE_DIR})
add_subdirectory(src)
