#include <tsl/grid.hpp>
#include <tsl/gl_buffer.hpp>

#include <utility>
#include <vector>

#include <glm/glm.hpp>

using glm::vec3;
using glm::cross;
using glm::normalize;

using std::vector;

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

    // one normal per vertex
    buffer.normal_buffer = vector<vec3>();
    auto& normal_buffer = *buffer.normal_buffer;
    normal_buffer.reserve(num_vertices);

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

    // generate normals TODO: approach is very naive! change it!
    for (uint32_t i = 0; i < y; ++i) {
        for (uint32_t j = 0; j < x; ++j) {

            const uint32_t current_index = j + (i * x);

            uint32_t i1 = current_index;
            uint32_t i2 = 0;
            uint32_t i3 = 0;

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

}
