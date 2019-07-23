cmake_minimum_required(VERSION 3.10)
project(tse VERSION 1.0.0 LANGUAGES CXX)

# enable C++17 standard
set(CMAKE_CXX_STANDARD 17)

# deps
find_package(OpenGL REQUIRED)
add_subdirectory(ext/imgui EXCLUDE_FROM_ALL)
include_directories(ext/imgui)
include_directories(ext/pfd)
add_subdirectory(ext/glfw EXCLUDE_FROM_ALL)
add_definitions(-DGLEW_STATIC)
add_subdirectory(ext/glew EXCLUDE_FROM_ALL)
add_subdirectory(../tsl tsl EXCLUDE_FROM_ALL)
include_directories(../tsl/include)

# application
set(TSE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${TSE_INCLUDE_DIR})
add_subdirectory(src)
