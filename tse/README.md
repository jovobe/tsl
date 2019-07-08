# T-Spline Editor
Editor for visualisation and manipulation of generalised T-Spline surfaces written in C++.

## build
In order to build this software you need a C++ compiler (compatible with C++17 standard) and the package manager [conan](https://conan.io/).

- `mkdir build && cd build`

### deps (conan)
- add remote: `conan remote add conan-bintray https://api.bintray.com/conan/bincrafters/public-conan`
- add remote: `conan remote add conan-community https://api.bintray.com/conan/conan-community/conan `
- build deps: `conan install .. --build missing`

### tse
- `cmake .. -DCMAKE_BUILD_TYPE=Release`
- `cmake --build .`

---

## License
Licensed under GNU General Public License v3.0 ([LICENSE](LICENSE) or https://opensource.org/licenses/GPL-3.0).

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, shall be licensed as above, without any additional terms or conditions.
