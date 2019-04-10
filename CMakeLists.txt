cmake_minimum_required(VERSION 3.10)
project(tsl VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# deps
find_package(OpenGL REQUIRED)

# imgui
add_subdirectory(ext/imgui)
include_directories(ext/imgui)

# application
set(TSL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${TSL_INCLUDE_DIR})
add_subdirectory(src)
add_subdirectory(test)

# TODO: This is a bad solution for loading shaders. Load them in a different way!
configure_file(shader/vertex.glsl shader/vertex.glsl COPYONLY)
configure_file(shader/fragment.glsl shader/fragment.glsl COPYONLY)

configure_file(shader/phong/vertex.glsl shader/phong/vertex.glsl COPYONLY)
configure_file(shader/phong/fragment.glsl shader/phong/fragment.glsl COPYONLY)

configure_file(shader/polygon/geometry.glsl shader/polygon/geometry.glsl COPYONLY)
configure_file(shader/vertex/geometry.glsl shader/vertex/geometry.glsl COPYONLY)

configure_file(shader/picking/vertex.glsl shader/picking/vertex.glsl COPYONLY)
configure_file(shader/picking/fragment.glsl shader/picking/fragment.glsl COPYONLY)
