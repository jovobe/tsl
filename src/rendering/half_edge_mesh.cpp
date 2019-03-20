#include <tsl/rendering/half_edge_mesh.hpp>
#include <tsl/attrmaps/attr_maps.hpp>
#include <tsl/rendering/picking_map.hpp>

namespace tsl {

gl_buffer get_edges_buffer(const half_edge_mesh& mesh, picking_map& picking_map) {
    gl_buffer out;
    out.vertex_buffer.reserve(mesh.num_edges() * 2);
    out.index_buffer.reserve(mesh.num_edges() * 2);
    out.picking_buffer = vector<uint32_t>();
    auto& picking_buffer = *out.picking_buffer;
    picking_buffer.reserve(mesh.num_edges() * 2);

    uint32_t current_index = 0;
    for (auto&& eh: mesh.get_edges()) {
        auto picking_id = picking_map.add_object(object_type::edge, eh);
        auto vertices = mesh.get_vertices_of_edge(eh);
        for (auto&& vh: vertices) {
            out.vertex_buffer.push_back(mesh.get_vertex_position(vh));
            picking_buffer.push_back(picking_id);
            out.index_buffer.push_back(current_index++);
        }
    }

    return out;
}

gl_buffer get_vertices_buffer(const half_edge_mesh& mesh, picking_map& picking_map) {
    gl_buffer out;
    out.vertex_buffer.reserve(mesh.num_vertices());
    out.index_buffer.reserve(mesh.num_vertices());
    out.picking_buffer = vector<uint32_t>();
    auto& picking_buffer = *out.picking_buffer;
    picking_buffer.reserve(mesh.num_vertices());

    uint32_t current_index = 0;
    for (auto&& vh: mesh.get_vertices()) {
        out.vertex_buffer.push_back(mesh.get_vertex_position(vh));
        out.index_buffer.push_back(current_index++);
        auto picking_id = picking_map.add_object(object_type::vertex, vh);
        picking_buffer.push_back(picking_id);
    }

    return out;
}

}
