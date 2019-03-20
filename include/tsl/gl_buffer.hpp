#ifndef TSL_GL_BUFFER_HPP
#define TSL_GL_BUFFER_HPP

#include <vector>
#include <optional>

#include <GL/glew.h>
#include <glm/glm.hpp>

using std::vector;
using std::optional;

using glm::vec3;

namespace tsl {

struct vertex_element {
    vec3 pos;
    vec3 normal;
    uint32_t picking_index;

    vertex_element() : pos(0, 0, 0), normal(0, 0, 0), picking_index(0) {}
    explicit vertex_element(const vec3& pos) : pos(pos), normal(0, 0, 0), picking_index(0) {}
};

struct gl_buffer {
    vector<vec3> vertex_buffer;
    vector<GLuint> index_buffer;
    optional<vector<vec3>> normal_buffer;
    optional<vector<uint32_t>> picking_buffer;

    vector<vertex_element> get_combined_vec_data() const;
};

struct gl_multi_buffer : public gl_buffer {
    vector<GLsizei> counts;
    vector<GLsizeiptr> indices;
};

}

#endif //TSL_GL_BUFFER_HPP
