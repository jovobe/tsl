#include <tsl/grid.hpp>
#include <tsl/gl_buffer.hpp>

#include <utility>

#include <glm/glm.hpp>

using glm::vec3;

namespace tsl {

gl_buffer regular_grid::get_render_buffer() const {
    gl_buffer buffer;

    // reserve space
    // TODO: assuming filled and regular here -> fix!
    auto x = static_cast<uint32_t>(points[0].size());
    auto y = static_cast<uint32_t>(points.size());
    auto num_vertices = x * y;

    // 3 values per vertex
    buffer.vertex_buffer.reserve(num_vertices);

    auto fields_x = x - 1;
    auto fields_y = y - 1;
    auto num_triangles = fields_x * fields_y * 2;
    auto num_indices = num_triangles * 3;

    // 3 indices per triangle
    buffer.index_buffer.reserve(num_indices);

    // copy vertices
    for (const auto& p : points) {
        for (const auto& v : p) {
            buffer.vertex_buffer.push_back(v);
        }
    }

    // generate indices
    for (uint32_t i = 0; i < y - 1; ++i) {
        for (uint32_t j = 0; j < x - 1; ++j) {

            const uint32_t current_index = j + (i * x);

            buffer.index_buffer.push_back(current_index);
            buffer.index_buffer.push_back(current_index + x);
            buffer.index_buffer.push_back(current_index + 1);

            buffer.index_buffer.push_back(current_index + 1);
            buffer.index_buffer.push_back(current_index + x);
            buffer.index_buffer.push_back(current_index + x + 1);
        }
    }

    return buffer;
}

}
