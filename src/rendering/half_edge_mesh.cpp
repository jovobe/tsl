#include <tsl/rendering/half_edge_mesh.hpp>
#include <tsl/attrmaps/attr_maps.hpp>

namespace tsl {

gl_buffer get_buffer(const half_edge_mesh& mesh) {
    gl_buffer out;
    out.vertex_buffer.reserve(mesh.num_vertices());
    out.index_buffer.reserve(mesh.num_edges() * 2);

    dense_vertex_map<uint32_t> index_map;
    index_map.reserve(mesh.num_vertices());

    uint32_t current_index = 0;
    for (auto&& eh: mesh.get_edges()) {
        auto vertices = mesh.get_vertices_of_edge(eh);
        for (auto&& vertex: vertices) {
            if (!index_map.contains_key(vertex)) {
                out.vertex_buffer.push_back(mesh.get_vertex_position(vertex));
                index_map.insert(vertex, current_index++);
            }

            out.index_buffer.push_back(index_map[vertex]);
        }
    }

    return out;
}

}
