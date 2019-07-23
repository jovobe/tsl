# T-Spline Library
Library for representation and manipulation of generalised T-Spline surfaces written in C++.

## Build
In order to build this software you need a C++ compiler (compatible with C++17 standard), CMake and git. Please make sure, that you cloned this repository with `--recursive` flag. If this is not the case, you need to run `git submodule update --init --recursive`.

### Dependencies
To build this library a compiled version of the BLAS standard is needed to link against. On macOS this is provided by the System. Other distribution can use [OpenBLAS](https://www.openblas.net/). If the BLAS library can not be found by CMake, an error message will be promted.

## Usage
The recommended way to embed this library is CMake. Just add:
```
add_definitions(-DTSL_EIGENVALUES_PREFX="ext/tsl/tsl/")
add_subdirectory(ext/tsl/tsl EXCLUDE_FROM_ALL)
include_directories(ext/tsl/tsl/include)

target_link_libraries(main PRIVATE tsl)
```
to your main `CMakeLists.txt`. This example assumes, that you will run your project from the root of your folder structure and that the tsl repository lies in `project_root/ext/tsl`.

You are now ready to calculate some points on a loaded quadmesh used as the tmesh like this:
```cpp
auto mesh = tsl::read_obj_into_tmesh(path_to_file);
auto evaluator = tsl::surface_evaluator(std::move(mesh));
auto grids = evaluator.eval_per_face(5);
auto first_grid = grids.front();
auto some_point = first_grid[0][0];
std::cout << some_point.x << ", " << some_point.y << ", " << some_point.z << std::endl;
```

For more examples just have a look into the code of the TSE.
