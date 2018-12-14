#ifndef TSL_GL_BUFFER_HPP
#define TSL_GL_BUFFER_HPP

#include <vector>

#include <glm/glm.hpp>

using std::vector;

using glm::vec3;

namespace tsl {

struct gl_buffer {
    vector<vec3> vertex_buffer;
    vector<uint32_t> index_buffer;
};

}

#endif //TSL_GL_BUFFER_HPP
