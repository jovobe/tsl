#ifndef TSL_GRID_HPP
#define TSL_GRID_HPP

#include <cstdint>
#include <vector>

#include <tsl/geometry/vector.hpp>
#include <tsl/geometry/tmesh/handles.hpp>

using std::vector;

namespace tsl {

struct regular_grid {
    vector<vector<vec3>> points;
    vector<vector<vec3>> normals;
    face_handle handle;
    size_t num_points_x;
    size_t num_points_y;

    explicit regular_grid(const face_handle& handle) : handle(handle), num_points_x(0), num_points_y(0) {}
};

}

#endif //TSL_GRID_HPP
