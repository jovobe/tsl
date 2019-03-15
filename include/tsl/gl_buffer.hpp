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

struct gl_buffer {
    vector<vec3> vertex_buffer;
    vector<GLuint> index_buffer;
    optional<vector<vec3>> normal_buffer;

    vector<vec3> get_combined_vec_data() const;
};

struct gl_multi_buffer : public gl_buffer {
    vector<GLsizei> counts;
    vector<GLsizeiptr> indices;
};

}

#endif //TSL_GL_BUFFER_HPP
