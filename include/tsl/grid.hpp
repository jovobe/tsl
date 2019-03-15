#ifndef TSL_GRID_HPP
#define TSL_GRID_HPP

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include <tsl/gl_buffer.hpp>

using std::vector;

using glm::vec3;

namespace tsl {

struct regular_grid {
    vector<vector<vec3>> points;

    /**
     * @return pair with <vertex buffer, index buffer>
     */
    gl_buffer get_render_buffer() const;
    gl_buffer add_to_render_buffer(gl_buffer& buffer) const;
};

gl_multi_buffer get_multi_render_buffer(const vector<regular_grid>& grids);

}

#endif //TSL_GRID_HPP
