#ifndef TSL_NEW_FACE_VERTEX_HPP
#define TSL_NEW_FACE_VERTEX_HPP

#include "handles.hpp"

namespace tsl {

/**
 * @brief A simple data container for the `tmesh::add_face` method to combine all needed data per vertex which should
 *        be added to the face.
 */
struct new_face_vertex {
    /// The vertex handle.
    vertex_handle handle;

    /// True, if the incoming edge to the vertex handle points into a face corner, false otherwise.
    bool corner;

    /// The knot interval which should be assigned to the incoming half edge of the vertex.
    double knot;

    /**
     * @brief The simplest ctor which assumes knot intervals of 1.0 and every vertex beeing a corner.
     *
     * @param handle The handle of the vertex which will be added to the face.
     */
    explicit new_face_vertex(const vertex_handle& handle) : handle(handle), corner(true), knot(1.0) {}

    /**
     * @brief Creates a new data container for the `tmesh::add_face` method by setting all of it's members.
     *
     * @param handle The handle of the vertex which will be added to the face.
     * @param corner True, if the incoming edge to the vertex points into a face corner, false otherwise.
     * @param knot The knot interval which should be assigned to the incoming half edge of the vertex.
     */
    new_face_vertex(const vertex_handle& handle, bool corner, double knot) : handle(handle), corner(corner), knot(knot) {}
};

}

#endif //TSL_NEW_FACE_VERTEX_HPP
