# tsl
Library for representation and manipulation of generalised T-Spline surfaces written in C++.

[![CircleCI](https://circleci.com/gh/jovobe/tsl.svg?style=svg)](https://circleci.com/gh/jovobe/tsl)

## build
In order to build this software you need a C++ compiler (compatible with C++17 standard) and the package manager [conan](https://conan.io/).

- `mkdir build && cd build`

### deps (conan)
- add remote: `conan remote add conan-bintray https://api.bintray.com/conan/bincrafters/public-conan`
- add remote: `conan remote add conan-community https://api.bintray.com/conan/conan-community/conan `
- build deps: `conan install .. --build`

### tsl
- `cmake .. -DCMAKE_BUILD_TYPE=Release`
- `cmake --build .`

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
