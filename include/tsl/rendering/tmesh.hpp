#ifndef TSL_RENDERING_TMESH_HPP
#define TSL_RENDERING_TMESH_HPP

#include <vector>
#include <set>

#include "tsl/grid.hpp"
#include "tsl/geometry/tmesh/tmesh.hpp"

using std::vector;
using std::set;

namespace tsl
{

vector<uint8_t> get_picked_faces_buffer(const vector<regular_grid>& faces, const set<picking_element>& picked);
vector<uint8_t> get_picked_edges_buffer(const tmesh& mesh, const set<picking_element>& picked);
vector<uint8_t> get_picked_vertices_buffer(const tmesh& mesh, const set<picking_element>& picked);

gl_buffer get_edges_buffer(const tmesh& mesh, picking_map& picking_map);
gl_buffer get_vertices_buffer(const tmesh& mesh, picking_map& picking_map);

}

#endif //TSL_RENDERING_TMESH_HPP
