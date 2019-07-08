#ifndef TSL_VERTEX_HPP
#define TSL_VERTEX_HPP

#include "handles.hpp"
#include "../vector.hpp"

namespace tsl {

// Forward declarations
struct optional_half_edge_handle;

/**
 * @brief Represents a vertex in the HEM data structure.
 */
struct vertex
{
    /// The edge starting at this vertex.
    optional_half_edge_handle outgoing;

    /// The 3D position of this vertex.
    vec3 pos;
};

}

#endif //TSL_VERTEX_HPP
