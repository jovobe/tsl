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

/**
 * @brief Creates a buffer with picking ids for faces from the given vector of
 * regular grids and a set of picked elements.
 */
vector<uint8_t> get_picked_faces_buffer(const vector<regular_grid>& faces, const set<picking_element>& picked);

/**
 * @brief Creates a buffer with picking ids for edges from the given tmesh and a set of picked elements.
 */
vector<uint8_t> get_picked_edges_buffer(const tmesh& mesh, const set<picking_element>& picked);

/**
 * @brief Creates a buffer with picking ids for vertices from the given tmesh and a set of picked elements.
 */
vector<uint8_t> get_picked_vertices_buffer(const tmesh& mesh, const set<picking_element>& picked);

/**
 * @brief Generates an OpenGL ready buffer for picking edges from the given tmesh and picking map.
 */
gl_buffer get_edges_buffer(const tmesh& mesh, picking_map& picking_map);

/**
 * @brief Generates an OpenGL ready buffer for picking vertices from the given tmesh and picking map.
 */
gl_buffer get_vertices_buffer(const tmesh& mesh, picking_map& picking_map);

}

#endif //TSL_RENDERING_TMESH_HPP
