# T-Spline Editor
Editor for representation and manipulation of generalised T-Spline surfaces written in C++. This uses the TSL to calculate points on the T-Spline surface, which can be found in the second project in this repository.

## Build
In order to build this software you need a C++ compiler (compatible with C++17 standard), CMake, OpenGL and git. Please make sure, that you cloned this repository with `--recursive` flag. If this is not the case, you need to run `git submodule update --init --recursive`.

- `mkdir build && cd build`
- `cmake .. -DCMAKE_BUILD_TYPE=Release`
- `cmake --build .`

## Run
To run the editor you have to start it from the root directory of the repository. If you used the
commands above the following would start the editor: `cd .. && ./build/src/tse`.

## Controls
The camera is moved with standard first person control via WASD. Space moves the camera up and
c moves it downwards. To speed up the movement you can hold shift. To select multiple elements in the
editor you can click them while holding down ctrl.
