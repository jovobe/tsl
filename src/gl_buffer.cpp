#include <tsl/gl_buffer.hpp>

#include <vector>

#include <glm/glm.hpp>

using glm::vec3;

using std::vector;

namespace tsl {

vector<vec3> gl_buffer::get_combined_vec_data() const {
    vector<vec3> vec_data;
    uint32_t times = 1;
    if (normal_buffer) {
        times += 1;
    }
    if (picking_buffer) {
        times += 1;
    }
    vec_data.reserve(vertex_buffer.size() * times);
    for (int i = 0; i < vertex_buffer.size(); ++i) {
        vec_data.push_back(vertex_buffer[i]);
        if (normal_buffer) {
            vec_data.push_back((*normal_buffer)[i]);
        }
        // TODO: Refactor gl_buffer to output a struct instead of vec3 (and use pragma omp parallel for)
        if (picking_buffer) {
            auto value = (*picking_buffer)[i];
            // TODO: Does this conversion (uint32_t -> float) preserve the bit pattern? Guaranteed?
            vec_data.emplace_back(value, 0.0f, 0.0f);
        }
    }
    return vec_data;
}

}
