#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include <tsl/geometry/vector.hpp>
#include <tsl/grid.hpp>

#include "tse/rendering/grid.hpp"
#include "tse/gl_buffer.hpp"
#include "tse/rendering/picking_map.hpp"

using std::vector;

using glm::cross;
using glm::normalize;
using glm::fvec3;

using tsl::regular_grid;

namespace tse {

gl_buffer get_render_buffer(const regular_grid& grid, picking_map& picking_map) {
    gl_buffer buffer;
    add_to_render_buffer(grid, buffer, picking_map);
    return buffer;
}

gl_buffer add_to_render_buffer(const regular_grid& grid, gl_buffer& buffer, picking_map& picking_map) {
    // reserve space
    auto x = static_cast<GLuint>(grid.num_points_x);
    auto y = static_cast<GLuint>(grid.num_points_y);
    auto num_vertices = x * y;
    auto old_num_vertices = static_cast<GLuint>(buffer.vertex_buffer.size());
    auto picking_id = picking_map.add_object(object_type::face, grid.handle);

    // 3 values per vertex
    buffer.vertex_buffer.reserve(buffer.vertex_buffer.size() + num_vertices);

    // one normal per vertex
    if (!buffer.normal_buffer) {
        buffer.normal_buffer = vector<fvec3>();
    }
    auto& normal_buffer = *buffer.normal_buffer;
    normal_buffer.reserve(normal_buffer.size() + num_vertices);

    // picking buffer
    if (!buffer.picking_buffer) {
        buffer.picking_buffer = vector<uint32_t>();
    }
    auto& picking_buffer = *buffer.picking_buffer;
    picking_buffer.reserve(picking_buffer.size() + num_vertices);

    auto fields_x = x - 1;
    auto fields_y = y - 1;
    auto num_triangles = fields_x * fields_y * 2;
    auto num_indices = num_triangles * 3;

    // 3 indices per triangle
    buffer.index_buffer.reserve(buffer.index_buffer.size() + num_indices);

    // copy vertices
    for (const auto& p : grid.points) {
        for (const auto& v : p) {
            buffer.vertex_buffer.emplace_back(v);
            picking_buffer.push_back(picking_id);
        }
    }

    // generate indices
    for (GLuint i = 0; i < y - 1; ++i) {
        for (GLuint j = 0; j < x - 1; ++j) {

            const GLuint current_index = (j + (i * x)) + old_num_vertices;

            buffer.index_buffer.push_back(current_index);
            buffer.index_buffer.push_back(current_index + x);
            buffer.index_buffer.push_back(current_index + 1);

            buffer.index_buffer.push_back(current_index + 1);
            buffer.index_buffer.push_back(current_index + x);
            buffer.index_buffer.push_back(current_index + x + 1);
        }
    }

    if (grid.normals.empty()) {
        // generate normals
        for (GLuint i = 0; i < y; ++i) {
            for (GLuint j = 0; j < x; ++j) {

                const GLuint current_index = (j + (i * x)) + old_num_vertices;

                GLuint i1 = current_index;
                GLuint i2 = 0;
                GLuint i3 = 0;

                // if at top right corner
                if (j == x - 1 && i == y - 1) {
                    i2 = current_index - x;
                    i3 = current_index - 1;
                }
                    // if at right border
                else if (j == x - 1) {
                    i2 = current_index + x - 1;
                    i3 = current_index + x;
                }
                    // if at top border
                else if (i == y - 1) {
                    i2 = current_index + 1;
                    i3 = current_index - x + 1;
                }
                    // std case
                else {
                    i2 = current_index + x;
                    i3 = current_index + 1;
                }

                auto& v1 = buffer.vertex_buffer[i1];
                auto& v2 = buffer.vertex_buffer[i2];
                auto& v3 = buffer.vertex_buffer[i3];
                normal_buffer.emplace_back(normalize(cross(v3 - v1, v2 - v1)));
            }
        }
    } else {
        // copy normals
        for (const auto& n: grid.normals) {
            for (const auto& v: n) {
                normal_buffer.emplace_back(v);
            }
        }
    }

    return buffer;
}

gl_multi_buffer get_multi_render_buffer(const vector<regular_grid>& grids, picking_map& picking_map) {
    gl_multi_buffer buffer;

    for (const auto& grid: grids) {
        auto old_count = buffer.index_buffer.size();
        buffer.indices.push_back(static_cast<GLsizeiptr>(buffer.index_buffer.size() * sizeof(GLuint)));
        add_to_render_buffer(grid, buffer, picking_map);
        buffer.counts.push_back(static_cast<GLsizei>(buffer.index_buffer.size() - old_count));
    }

    return buffer;
}

}
