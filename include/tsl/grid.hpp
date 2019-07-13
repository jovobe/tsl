#ifndef TSL_GRID_HPP
#define TSL_GRID_HPP

#include <cstdint>
#include <vector>

#include "tsl/gl_buffer.hpp"
#include "tsl/rendering/picking_map.hpp"
#include "tsl/geometry/vector.hpp"

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

    /**
     * @brief Converts this grid into a `gl_buffer`.
     */
    gl_buffer get_render_buffer(picking_map& picking_map) const;

    /**
     * @brief Adds this grid into the given `gl_buffer`.
     */
    gl_buffer add_to_render_buffer(gl_buffer& buffer, picking_map& picking_map) const;
};

/**
 * @brief Converts a vector of `regular_grid` into a `gl_multi_buffer`.
 */
gl_multi_buffer get_multi_render_buffer(const vector<regular_grid>& grids, picking_map& picking_map);

}

#endif //TSL_GRID_HPP
