#ifndef TSL_FACE_HPP
#define TSL_FACE_HPP

#include "handles.hpp"

namespace tsl {

// Forward declarations
struct half_edge_handle;

/**
 * @brief Represents a face in the tmesh data structure.
 */
struct face
{
    explicit face(half_edge_handle edge)
        : edge(edge) {}

    /// One of the edges bounding this face.
    half_edge_handle edge;
};

}

#endif //TSL_FACE_HPP
