cmake_minimum_required(VERSION 3.12)
project(tsl VERSION 1.0.0 LANGUAGES CXX)

# Check for dependencies
#if(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/ext/nanogui/ext/glfw")
#    message(FATAL_ERROR "The TSL dependency repositories (NanoGUI, GLFW, etc.) are missing! "
#            "You probably did not clone the project with --recursive. It is possible to recover "
#            "by calling \"git submodule update --init --recursive\"")
#endif()

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
#find_package(Freetype REQUIRED)

# glfreetype
#add_subdirectory(ext/glfreetype)
#include_directories(ext/glfreetype/include)

include_directories(include)

add_subdirectory(src)
add_subdirectory(test)

configure_file(shader/vertex.glsl shader/vertex.glsl COPYONLY)
configure_file(shader/fragment.glsl shader/fragment.glsl COPYONLY)
