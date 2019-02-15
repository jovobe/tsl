#ifndef TSL_HALF_EDGE_HPP
#define TSL_HALF_EDGE_HPP

#include "handles.hpp"

namespace tsl {

struct half_edge
{
    /// The face this edge belongs to (or none, if this edge lies on the
    /// boundary).
    optional_face_handle face;

    /// The vertex this edge points to.
    vertex_handle target;

    /// The next edge of the face, ordered counter-clockwise. Viewed a different
    /// way: it's the next edge when walking clockwise around the source
    /// vertex.
    half_edge_handle next;

    /// The twin edge.
    half_edge_handle twin;

private:
    /**
     * @brief Initializes all fields with dummy values (unsafe, thus private).
     */
    half_edge() : target(0), next(0), twin(0) {}

    /// Several methods of HEM need to invoke the unsafe ctor.
    friend class half_edge_mesh;
};

}

#endif //TSL_HALF_EDGE_HPP
