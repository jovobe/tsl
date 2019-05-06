#include <algorithm>
#include <iterator>

#include <tsl/rendering/half_edge_mesh.hpp>
#include <tsl/attrmaps/attr_maps.hpp>
#include <tsl/rendering/picking_map.hpp>

using std::copy_if;
using std::back_inserter;
using std::transform;
using std::find;

namespace tsl {

vector<uint8_t> get_picked_edges_buffer(const half_edge_mesh& mesh, const set<picking_element>& picked) {

    // Filter type edges
    vector<reference_wrapper<const picking_element>> edges_picked;
    copy_if(picked.begin(), picked.end(), back_inserter(edges_picked), [](const picking_element& elem) {
        return elem.type == object_type::edge;
    });

    // If no edges are selected, return all zero vec
    if (edges_picked.empty()) {
        return vector<uint8_t>(mesh.num_edges() * 2, 0);
    }

    // Transform handles into edge handles
    vector<edge_handle> picked_handles;
    transform(edges_picked.begin(), edges_picked.end(), back_inserter(picked_handles), [](const picking_element& elem) {
        return edge_handle(elem.handle.get_idx());
    });

    // Get selected vertices
    vector<uint8_t> out;
    out.reserve(mesh.num_edges() * 2);
    for (auto&& eh: mesh.get_edges()) {
        auto found = find(picked_handles.begin(), picked_handles.end(), eh);
        auto picked_val = static_cast<uint8_t>(found != picked_handles.end());
        auto vertices = mesh.get_vertices_of_edge(eh);
        for (auto&& vh: vertices) {
            out.push_back(picked_val);
        }
    }

    return out;
}

vector<uint8_t> get_picked_vertices_buffer(const half_edge_mesh& mesh, const set<picking_element>& picked) {
    // Filter type vertices
    vector<reference_wrapper<const picking_element>> vertices_picked;
    copy_if(picked.begin(), picked.end(), back_inserter(vertices_picked), [](const picking_element& elem) {
        return elem.type == object_type::vertex;
    });

    // If no vertices are selected, return all zero vec
    if (vertices_picked.empty()) {
        return vector<uint8_t>(mesh.num_vertices(), 0);
    }

    // Transform handles into vertex handles
    vector<vertex_handle> picked_handles;
    transform(vertices_picked.begin(), vertices_picked.end(), back_inserter(picked_handles), [](const picking_element& elem) {
        return vertex_handle(elem.handle.get_idx());
    });

    // Get selected vertices
    vector<uint8_t> out;
    out.reserve(mesh.num_vertices());
    for (auto&& vh: mesh.get_vertices()) {
        auto found = find(picked_handles.begin(), picked_handles.end(), vh);
        auto picked_val = static_cast<uint8_t>(found != picked_handles.end());
        out.push_back(picked_val);
    }

    return out;
}

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
            out.vertex_buffer.emplace_back(mesh.get_vertex_position(vh));
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
        out.vertex_buffer.emplace_back(mesh.get_vertex_position(vh));
        out.index_buffer.push_back(current_index++);
        auto picking_id = picking_map.add_object(object_type::vertex, vh);
        picking_buffer.push_back(picking_id);
    }

    return out;
}

}
