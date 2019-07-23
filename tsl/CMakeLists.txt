cmake_minimum_required(VERSION 3.10)
project(tsl VERSION 1.0.0 LANGUAGES CXX)

# enable C++17 standard
set(CMAKE_CXX_STANDARD 17)

# deps
find_package(BLAS REQUIRED)
include_directories(ext/tinyobjloader)
if (NOT TARGET glm)
    add_subdirectory(ext/glm EXCLUDE_FROM_ALL)
endif()
if (NOT TARGET fmt)
    add_subdirectory(ext/fmt EXCLUDE_FROM_ALL)
endif()

# application
set(TSL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${TSL_INCLUDE_DIR})
add_subdirectory(src)
add_subdirectory(test)
