#ifndef TSL_GRID_HPP
#define TSL_GRID_HPP

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include <tsl/gl_buffer.hpp>
#include <tsl/rendering/picking_map.hpp>

using std::vector;

using glm::vec3;

namespace tsl {

struct regular_grid {
    vector<vector<vec3>> points;
    face_handle handle;
    size_t num_points_x;
    size_t num_points_y;

    explicit regular_grid(const face_handle& handle) : handle(handle), num_points_x(0), num_points_y(0) {}

    /**
     * @return pair with <vertex buffer, index buffer>
     */
    gl_buffer get_render_buffer(picking_map& picking_map) const;
    gl_buffer add_to_render_buffer(gl_buffer& buffer, picking_map& picking_map) const;
};

gl_multi_buffer get_multi_render_buffer(const vector<regular_grid>& grids, picking_map& picking_map);

}

#endif //TSL_GRID_HPP
