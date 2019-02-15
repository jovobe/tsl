#ifndef TSL_HALF_EDGE_VERTEX_HPP
#define TSL_HALF_EDGE_VERTEX_HPP

#include <glm/glm.hpp>

#include "handles.hpp"

using glm::vec3;

namespace tsl {

// Forward declarations
struct optional_half_edge_handle;

/**
 * @brief Represents a vertex in the HEM data structure.
 */
struct half_edge_vertex
{
    /// The edge starting at this vertex.
    optional_half_edge_handle outgoing;

    /// The 3D position of this vertex.
    vec3 pos;
};

}

#endif //TSL_HALF_EDGE_VERTEX_HPP
