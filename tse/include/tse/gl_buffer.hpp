#ifndef TSE_GL_BUFFER_HPP
#define TSE_GL_BUFFER_HPP

#include <vector>
#include <optional>

#include <GL/glew.h>

#include <glm/glm.hpp>

using std::vector;
using std::optional;

using glm::fvec3;

namespace tse {

/**
 * @brief POD type for holding all relevant infos for a vertex which will be passed to a shader.
 */
struct vertex_element {
    /// 3d position of the vertex.
    fvec3 pos;
    /// Normal direction of the vertex.
    fvec3 normal;
    /// Index in the picking map of the vertex.
    uint32_t picking_index;

    vertex_element() : pos(0, 0, 0), normal(0, 0, 0), picking_index(0) {}
    explicit vertex_element(const fvec3& pos) : pos(pos), normal(0, 0, 0), picking_index(0) {}
};

/**
 * @brief Represents a buffer for the OpenGL context.
 */
struct gl_buffer {
    /// The buffer holding all 3d positions.
    vector<fvec3> vertex_buffer;
    /// The index buffer according to the `vertex_buffer`.
    vector<GLuint> index_buffer;
    /// The buffer holding all normal directions. This has the same indices as the `vertex_buffer`.
    optional<vector<fvec3>> normal_buffer;
    /// The buffer holding all pickind ids. This has the same indices as the `vertex_buffer`.
    optional<vector<uint32_t>> picking_buffer;

    /**
     * @brief Converts the data inside this buffer to a vector of `vertex_element` which can be passed to OpenGL.
     */
    vector<vertex_element> get_combined_vec_data() const;
};

/**
 * @brief Represents a multi draw buffer for the OpenGL context.
 */
struct gl_multi_buffer : public gl_buffer {
    /// The number of all buffer sizes which this multi buffer contains.
    vector<GLsizei> counts;
    /// The beginning of all index buffer inside this multi buffer.
    vector<GLsizeiptr> indices;
};

}

#endif //TSE_GL_BUFFER_HPP
