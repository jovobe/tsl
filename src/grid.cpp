#include <tsl/grid.hpp>
#include <tsl/gl_buffer.hpp>
#include <tsl/rendering/picking_map.hpp>

#include <utility>
#include <vector>

#include <glm/glm.hpp>

using glm::vec3;
using glm::cross;
using glm::normalize;

using std::vector;

namespace tsl {

gl_buffer regular_grid::get_render_buffer(picking_map& picking_map) const {
    gl_buffer buffer;
    add_to_render_buffer(buffer, picking_map);
    return buffer;
}

gl_buffer regular_grid::add_to_render_buffer(gl_buffer& buffer, picking_map& picking_map) const {
    // reserve space
    // TODO: assuming filled and regular here -> fix!
    auto x = static_cast<GLuint>(num_points_x);
    auto y = static_cast<GLuint>(num_points_y);
    auto num_vertices = x * y;
    auto old_num_vertices = static_cast<GLuint>(buffer.vertex_buffer.size());
    auto picking_id = picking_map.add_object(object_type::face, handle);

    // 3 values per vertex
    buffer.vertex_buffer.reserve(buffer.vertex_buffer.size() + num_vertices);

    // one normal per vertex
    if (!buffer.normal_buffer) {
        buffer.normal_buffer = vector<vec3>();
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
    for (const auto& p : points) {
        for (const auto& v : p) {
            buffer.vertex_buffer.push_back(v);
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

    // generate normals TODO: approach is very naive! change it!
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

    return buffer;
}

gl_multi_buffer get_multi_render_buffer(const vector<regular_grid>& grids, picking_map& picking_map) {
    gl_multi_buffer buffer;

    for (auto&& grid: grids) {
        auto old_count = buffer.index_buffer.size();
        buffer.indices.push_back(static_cast<GLsizeiptr>(buffer.index_buffer.size() * sizeof(GLuint)));
        grid.add_to_render_buffer(buffer, picking_map);
        buffer.counts.push_back(static_cast<GLsizei>(buffer.index_buffer.size() - old_count));
    }

    return buffer;
}

}
