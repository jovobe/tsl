#ifndef TSL_GRID_HPP
#define TSL_GRID_HPP

#include <cstdint>
#include <vector>

#include "tsl/geometry/vector.hpp"
#include "tsl/geometry/tmesh/handles.hpp"

using std::vector;

namespace tsl {

/**
 * @brief Represents a regular grid of points with normals, which belongs to a face in the tmesh.
 */
struct regular_grid {
    /// All points of the grid.
    vector<vector<vec3>> points;
    /// All normals of the grid.
    vector<vector<vec3>> normals;
    /// The handle of the face this grid belongs to.
    face_handle handle;
    /// The number of points in x direction.
    size_t num_points_x;
    /// The number of points in y direction.
    size_t num_points_y;

    explicit regular_grid(const face_handle& handle) : handle(handle), num_points_x(0), num_points_y(0) {}
};

}

#endif //TSL_GRID_HPP
