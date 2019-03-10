#ifndef TSL_RENDERING_HALF_EDGE_MESH_HPP
#define TSL_RENDERING_HALF_EDGE_MESH_HPP

#include <tsl/gl_buffer.hpp>
#include <tsl/geometry/half_edge_mesh.hpp>

namespace tsl {

gl_buffer get_buffer(const half_edge_mesh& mesh);

}

#endif //TSL_RENDERING_HALF_EDGE_MESH_HPP
