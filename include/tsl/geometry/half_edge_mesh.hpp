#ifndef TSL_HALF_EDGE_MESH_HPP
#define TSL_HALF_EDGE_MESH_HPP

#include <vector>
#include <array>
#include <utility>

#include <tsl/attrmaps/stable_vector.hpp>
#include "half_edge.hpp"
#include "half_edge_face.hpp"
#include "half_edge_vertex.hpp"

using std::vector;
using std::array;
using std::pair;

namespace tsl {

/**
 * @brief Half-edge data structure.
 *
 * This implementation encodes many connectivity details explicitly, enabling fast lookup.
 */
class half_edge_mesh {
public:
    using edge = half_edge;
    using face = half_edge_face;
    using vertex = half_edge_vertex;

    half_edge_mesh() = default;

    /**
     * @brief Adds a vertex with the given position to the mesh.
     *
     * The vertex is not connected to anything after calling this method. To
     * add this vertex to a face, use `add_face()`.
     *
     * @return A handle to access the inserted vertex later.
     */
    vertex_handle add_vertex(vec3 pos);

    /**
     * @brief Creates a face connecting the given vertices.
     *
     * Important: The face's vertices have to be given in front-face counter-
     * clockwise order. This means that, when looking at the face's front, the
     * vertices would appear in counter-clockwise order. Or in more mathy
     * terms: the face's normal is equal to (v1 - v2) x (v1 - v3) in the
     * right-handed coordinate system (where `x` is cross-product).
     *
     * This method panics if an insertion is not possible. You can check
     * whether or not an insertion is valid by using `is_face_insertion_valid()`.
     *
     * @return A handle to access the inserted face later.
     */
    face_handle add_face(const vector<vertex_handle>& handles);

    /**
     * @brief Returns the number of vertices in the mesh.
     */
    size_t num_vertices() const;

    /**
     * @brief Returns the number of faces in the mesh.
     */
    size_t num_faces() const;

    /**
     * @brief Returns the number of edges (not half-edges!) in the mesh.
     */
    size_t num_edges() const;

    /**
     * @brief Get the position of the given vertex.
     */
    vec3 get_vertex_position(vertex_handle handle) const;

    /**
     * @brief Get a ref to the position of the given vertex.
     */
    vec3& get_vertex_position(vertex_handle handle);

    /**
     * @brief Get the vertices surrounding the given face.
     *
     * @return The vertex-handles in counter-clockwise order.
     */
    vector<vertex_handle> get_vertices_of_face(face_handle handle) const;

    /**
     * @brief Get the two vertices of an edge.
     *
     * The order of the vertices is not specified
     */
    array<vertex_handle, 2> get_vertices_of_edge(edge_handle edge_h) const;

    /**
     * @brief Get the two faces of an edge.
     *
     * The order of the faces is not specified
     */
    array<optional_face_handle, 2> get_faces_of_edge(edge_handle edge_h) const;

    /**
     * @brief Get a list of edges around the given vertex.
     *
     * The edge handles are written into the `edges_out` vector. This is done
     * to reduce the number of heap allocations if this method is called in
     * a loop. If you are not calling it in a loop or can't, for some reason,
     * take advantages of this method's signature, you can call the other
     * overload of this method which just returns the vector. Such convinient.
     *
     * Note: you probably should remember to `clear()` the vector before
     * passing it into this method.
     *
     * @param edges_out The handles of the edges around `handle` will be written
     *                 into this vector in clockwise order.
     */
    void get_edges_of_vertex(vertex_handle handle, vector<edge_handle>& edges_out) const;

    /**
     * @brief Check whether or not inserting a face between the given vertices
     *        would be valid.
     *
     * Adding a face is invalid if it destroys the mesh in any kind, like
     * making it non-manifold, non-orientable or something similar. But there
     * are other reasons for invalidity as well, like: there is already a face
     * connecting the given vertices.
     *
     * Note that the given vertices have to be in front-face counter-clockwise
     * order, just as with `add_face()`. See `add_face()` for more information
     * about this.
     */
    bool is_face_insertion_valid(const vector<vertex_handle>& handles) const;

    /**
     * @brief If all vertices are part of one face, this face is returned. None
     *        otherwise.
     *
     * The vertices don't have to be in a specific order. In particular, this
     * method will find a face regardless of whether the vertices are given in
     * clockwise or counter-clockwise order.
     */
    optional_face_handle get_face_between(const vector<vertex_handle>& handles) const;

    /**
     * @brief Returns the number of adjacent faces to the given edge.
     *
     * This functions always returns one of 0, 1 or 2.
     */
    uint8_t num_adjacent_faces(edge_handle handle) const;

    /**
     * @brief Get a list of edges around the given vertex.
     *
     * This method is implemented using the pure virtual method
     * `get_edges_of_vertex(vertex_handle, vector<edge_handle>&)`. If you are
     * calling this method in a loop, you should probably call the more manual
     * method (with the out vector) to avoid useless heap allocations.
     *
     * @return The edge-handles in counter-clockwise order.
     */
    vector<edge_handle> get_edges_of_vertex(vertex_handle handle) const;

    /**
     * @brief If the two given vertices are connected by an edge, it is
     *        returned. None otherwise.
     */
    optional_edge_handle get_edge_between(vertex_handle ah, vertex_handle bh) const;

private:
    stable_vector<half_edge_handle, edge> edges;
    stable_vector<face_handle, face> faces;
    stable_vector<vertex_handle, vertex> vertices;

    // ========================================================================
    // = Private helper methods
    // ========================================================================
    edge& get_e(half_edge_handle handle);
    const edge& get_e(half_edge_handle handle) const;
    face& get_f(face_handle handle);
    const face& get_f(face_handle handle) const;
    vertex& get_v(vertex_handle handle);
    const vertex& get_v(vertex_handle handle) const;

    /**
     * @brief Converts a half edge handle to a full edge handle
     *
     * @return  The handle with the smaller index of the given half edge and
     *          its twin
     */
    edge_handle half_to_full_edge_handle(half_edge_handle handle) const;

    /**
     * @brief Given two vertices, find the edge pointing from one to the other.
     *
     * @return None, if there exists no such edge.
     */
    optional_half_edge_handle edge_between(vertex_handle from_h, vertex_handle to_h);

    /**
     * @brief Attempts to find an edge between the given vertices and, if none
     *        is found, creates a new edge with `add_edge_pair()`
     *
     * @return The half edge from `from_h` to `to_h`
     */
    half_edge_handle find_or_create_edge_between(vertex_handle from_h, vertex_handle to_h);

    /**
     * @brief Adds a new, incomplete edge-pair.
     *
     * This method is private and unsafe, because it leaves some fields
     * uninitialized. The invariants of this mesh are broken after calling this
     * method and the caller has to fix those broken invariants.
     *
     * In particular, no `next` handle is set or changed. The `outgoing` handle
     * of the vertices is not changed (or set) either.
     *
     * @return Both edge handles. The first edge points from v1h to v2h, the
     *         second one points from v2h to v1h.
     */
    pair<half_edge_handle, half_edge_handle> add_edge_pair(vertex_handle v1h, vertex_handle v2h);

    /**
     * @brief Circulates around the vertex `vh`, calling the `visitor` for each
     *        ingoing edge of the vertex.
     *
     * The edges are visited in clockwise order. The iteration stops if all
     * edges were visited once or if the visitor returns `false`. It has to
     * return `true` to keep circulating. If the vertex has no outgoing edge,
     * this method does nothing.
     */
    template <typename visitor_t>
    void circulate_around_vertex(vertex_handle vh, visitor_t visitor) const;

    /**
     * @brief Circulates around the vertex `start_edge_h.target`, calling the
     *        `visitor` for each ingoing edge of the vertex.
     *
     * This works exactly as the other overload, but specifically starts at the
     * edge `start_edge_h` instead of `vh.outgoing.twin`.
     */
    template <typename visitor_t>
    void circulate_around_vertex(half_edge_handle start_edge_h, visitor_t visitor) const;

    /**
     * @brief Iterates over all ingoing edges of one vertex, returning the
     *        first edge that satisfies the given predicate.
     *
     * @return Returns None if `v` does not have an outgoing edge or if no
     *         edge in the circle satisfies the predicate.
     */
    template <typename pred_t>
    optional_half_edge_handle find_edge_around_vertex(vertex_handle vh, pred_t pred) const;

    /**
     * @brief Iterates over all ingoing edges of the vertex `start_edge.target`,
     *        starting at the edge `start_edge_h`, returning the first edge that
     *        satisfies the given predicate.
     *
     * @return Returns None if no edge in the circle satisfies the predicate.
     */
    template <typename pred_t>
    optional_half_edge_handle find_edge_around_vertex(half_edge_handle start_edge_h, pred_t pred) const;
};

}

#endif //TSL_HALF_EDGE_MESH_HPP
