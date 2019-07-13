#ifndef INCLUDE_TSL_ALGORITHM_GET_VERTICES_HPP
#define INCLUDE_TSL_ALGORITHM_GET_VERTICES_HPP

#include <vector>

#include "../geometry/tmesh/handles.hpp"
#include "../geometry/tmesh/tmesh.hpp"

using std::vector;

namespace tsl {

/**
 * @brief Returns the n regular rings around the given vertex.
 */
vector<vertex_handle> get_regular_rings_around_vertex(const tmesh& mesh, vertex_handle handle, uint32_t rings);

/**
 * @brief Returns all extraordinary vertices in the n regular rings around the given vertex.
 */
vector<vertex_handle> get_extraordinary_vertices_in_regular_rings_around_vertex(const tmesh& mesh, vertex_handle handle, uint32_t rings);

/**
 * @brief Takes a visitor, which visits all vertices in the n regular rings around the given vertex.
 *
 * @param visitor This has to have the signature: `(vertex_handle, uint32_t) -> bool`. If it returns false, the method
 *                will stop calling the visitor.
 */
template <typename visitor_t>
void visit_regular_rings(const tmesh& mesh, vertex_handle handle, uint32_t rings, visitor_t visitor);

}

#include "get_vertices.tcc"

#endif //INCLUDE_TSL_ALGORITHM_GET_VERTICES_HPP
