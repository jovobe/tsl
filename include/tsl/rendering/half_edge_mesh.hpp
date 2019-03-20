#ifndef TSL_RENDERING_HALF_EDGE_MESH_HPP
#define TSL_RENDERING_HALF_EDGE_MESH_HPP

#include <tsl/gl_buffer.hpp>
#include <tsl/geometry/half_edge_mesh.hpp>
#include <tsl/rendering/picking_map.hpp>

namespace tsl {

gl_buffer get_edges_buffer(const half_edge_mesh& mesh, picking_map& picking_map);
gl_buffer get_vertices_buffer(const half_edge_mesh& mesh, picking_map& picking_map);

}

#endif //TSL_RENDERING_HALF_EDGE_MESH_HPP
