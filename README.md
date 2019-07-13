# T-Spline Library & T-Spline Editor
Library and editor for representation and manipulation of generalised T-Spline surfaces written in C++.

[![CircleCI](https://circleci.com/gh/jovobe/tsl.svg?style=svg)](https://circleci.com/gh/jovobe/tsl)

## Build
In order to build this software you need a C++ compiler (compatible with C++17 standard) and the package manager [conan](https://conan.io/).

- `mkdir build && cd build`

### deps (conan)
- add remote: `conan remote add conan-bintray https://api.bintray.com/conan/bincrafters/public-conan`
- add remote: `conan remote add conan-community https://api.bintray.com/conan/conan-community/conan `
- build deps: `conan install .. --build missing`

### Editor
- `cmake .. -DCMAKE_BUILD_TYPE=Release`
- `cmake --build .`

## Run
To run the editor you have to start it from the root directory of the repository. If you used the
commands above the following would start the editor: `./build/bin/tsl_main`.

## Controls
The camera is moved with standard first person control via WASD. Space moves the camera up and
c moves it downwards. To speed up the movement you can hold shift. To select multiple elements in the
editor you can click them while holding down ctrl.

---

## License
Licensed under either of

 * Apache License, Version 2.0, ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
 * MIT license ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.

### Contribution
Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall
be dual licensed as above, without any additional terms or conditions.
