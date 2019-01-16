#include <tsl/gl_buffer.hpp>

#include <vector>

#include <glm/glm.hpp>

using glm::vec3;

using std::vector;

namespace tsl {

vector<vec3> gl_buffer::get_combined_vec_data() const {
    vector<vec3> vec_data;
    vec_data.reserve(this->vertex_buffer.size() * 2);
    for (int i = 0; i < this->vertex_buffer.size(); ++i) {
        vec_data.push_back(this->vertex_buffer[i]);
        // TODO: check for normals!
        vec_data.push_back((*this->normal_buffer)[i]);
    }
    return vec_data;
}

}
