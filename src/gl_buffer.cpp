#include <tsl/gl_buffer.hpp>

#include <vector>

using std::vector;

namespace tsl {

vector<vertex_element> gl_buffer::get_combined_vec_data() const {
    vector<vertex_element> vec_data;
    vec_data.reserve(vertex_buffer.size());

    for (size_t i = 0; i < vertex_buffer.size(); ++i) {

        vertex_element elem(vertex_buffer[i]);

        if (normal_buffer) {
            elem.normal = (*normal_buffer)[i];
        }

        if (picking_buffer) {
            elem.picking_index = (*picking_buffer)[i];
        }

        vec_data.push_back(elem);
    }
    return vec_data;
}

}
