#ifndef TSL_HALF_EDGE_FACE_HPP
#define TSL_HALF_EDGE_FACE_HPP

#include "handles.hpp"

namespace tsl {

// Forward declarations
struct half_edge_handle;

/**
 * @brief Represents a face in the HEM data structure.
 */
struct half_edge_face
{
    explicit half_edge_face(half_edge_handle edge)
        : edge(edge) {}

    /// One of the edges bounding this face.
    half_edge_handle edge;
};

}

#endif //TSL_HALF_EDGE_FACE_HPP
