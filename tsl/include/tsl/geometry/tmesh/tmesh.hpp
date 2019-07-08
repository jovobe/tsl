#ifndef TSL_TMESH_HPP
#define TSL_TMESH_HPP

#include <vector>
#include <set>
#include <array>
#include <utility>
#include <optional>

#include "tsl/attrmaps/stable_vector.hpp"
#include "../vector.hpp"
#include "handles.hpp"
#include "edge_direction.hpp"
#include "iterator.hpp"
#include "half_edge.hpp"
#include "face.hpp"
#include "vertex.hpp"
#include "new_face_vertex.hpp"

using std::vector;
using std::set;
using std::array;
using std::pair;
using std::optional;

namespace tsl {

// Forward declaration
template<typename handle_t>
class tmesh_iterator_ptr;
class tmesh_face_iterator_proxy;
class tmesh_half_edge_iterator_proxy;
class tmesh_edge_iterator_proxy;
class tmesh_vertex_iterator_proxy;

/**
 * @brief This is an implementation of a T-Mesh based on a half edge mesh.
 */
class tmesh {
public:
    // ========================================================================
    // = Modifier
    // ========================================================================

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
    bool is_face_insertion_valid(const vector<new_face_vertex>& new_vertices) const;

    /**
     * @brief This is a convenience function to check the insertion of a regular quad face to the mesh.
     *
     * This creates a `new_face_vertex` per handle with its default ctor and than checks insertion of the face via
     * `tmesh::is_face_insertion_valid(const vector<new_face_vertex>&)`. Knot values will be 1.0 for all inner edges.
     */
    bool is_face_insertion_valid(const array<vertex_handle, 4>& new_vertices) const;

    /**
     * @brief Creates a face connecting the given vertices.
     *
     * Important: The face's vertices have to be given in front-face counter-
     * clockwise order. This means that, when looking at the face's front, the
     * vertices would appear in counter-clockwise order. Or in more mathy
     * terms: the face's normal is equal to (v1 - v2) x (v1 - v3) in the
     * right-handed coordinate system (where `x` is cross-product).
     *
     * Important: The knot and corner values from `new_face_vertex` will be applied to the
     * inner half edges.
     *
     * This method panics if an insertion is not possible. You can check
     * whether or not an insertion is valid by using `is_face_insertion_valid()`.
     *
     * @return A handle to access the inserted face later.
     */
    face_handle add_face(const vector<new_face_vertex>& new_vertices);

    /**
     * @brief This is a convenience function to add a regular quad face to the mesh.
     *
     * This creates a `new_face_vertex` per handle with its default ctor and than adds the face via
     * `tmesh::add_face(const vector<new_face_vertex>&)`. Knot values will be 1.0 for all inner edges.
     */
    face_handle add_face(const array<vertex_handle, 4>& new_vertices);

    /**
     * @brief Removes the given edge and one of the two faces it seperades and returns true. Returns false, if the
     *        edge could not be deleted.
     */
    bool remove_edge(edge_handle handle);

    // ========================================================================
    // = Get numbers
    // ========================================================================

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
     * @brief Returns the number of half edges in the mesh.
     */
    size_t num_half_edges() const;

    /**
     * @brief Returns the number of adjacent faces to the given edge.
     *
     * This functions always returns one of 0, 1 or 2.
     */
    uint8_t num_adjacent_faces(edge_handle handle) const;

    /**
     * @brief Returns the number of direct neighbours of the vertex.
     */
    size_t get_valence(vertex_handle handle) const;

    /**
     * @brief Returns the number of direct neighbours of the vertex plus the faces, for which the vertex is no corner.
     */
    size_t get_extended_valence(vertex_handle handle) const;

    /**
     * @brief Returns true, if the vertex has an extended valence which is not equal to 4, false otherwise.
     */
    bool is_extraordinary(vertex_handle handle) const;

    // ========================================================================
    // = Get attributes
    // ========================================================================

    /**
     * @brief Get the position of the given vertex.
     */
    vec3 get_vertex_position(vertex_handle handle) const;

    /**
     * @brief Get a ref to the position of the given vertex.
     */
    vec3& get_vertex_position(vertex_handle handle);

    /**
     * @brief Returns the knot interval associated with the given half edge.
     */
    optional<double> get_knot_interval(half_edge_handle handle) const;

    /**
     * @brief Returns true, if the given half edge points into a face corner, false otherwise.
     */
    optional<bool> corner(half_edge_handle handle) const;

    /**
     * @brief Returns true, if the given half edge points away from a face corner, false otherwise.
     */
    optional<bool> from_corner(half_edge_handle handle) const;

    /**
     * @brief Returns the knot interval from the requested half edge divided by the knot interval from the twin of
     *        the requested half edge.
     */
    optional<double> get_knot_factor(half_edge_handle handle) const;

    // ========================================================================
    // = Follow pointer
    // ========================================================================

    /**
     * @brief Returns the twin of the requested half edge.
     */
    half_edge_handle get_twin(half_edge_handle handle) const;

    /**
     * @brief Returns the next half edge the given half edge points to.
     */
    half_edge_handle get_next(half_edge_handle handle) const;

    /**
     * @brief Returns the half edge which points to the given half edge.
     */
    half_edge_handle get_prev(half_edge_handle handle) const;

    /**
     * @brief Get handle of the vertex the given half edge handle points to.
     */
    vertex_handle get_target(half_edge_handle handle) const;

    /**
     * @brief Get handle of the half edge the given vertex points to.
     */
    optional_half_edge_handle get_out(vertex_handle handle) const;

    /**
     * @brief Get handle of the half edge the given face points to.
     */
    half_edge_handle get_edge(face_handle handle) const;

    // ========================================================================
    // = Get vertices
    // ========================================================================

    /**
     * @brief Get the vertices surrounding the given face.
     *
     * @return The vertex-handles in counter-clockwise order.
     */
    vector<vertex_handle> get_vertices_of_face(face_handle handle) const;

    /**
     * @brief Adds the vertices surrounding the given face to the given vector.
     *
     * @return The vertex-handles in counter-clockwise order. The vertex handles are written
     * into the `vertices_out` vector. This is done
     * to reduce the number of heap allocations if this method is called in
     * a loop. If you are not calling it in a loop or can't, for some reason,
     * take advantages of this method's signature, you can call the other
     * overload of this method which just returns the vector. Such convinient.
     *
     * Note: you probably should remember to `clear()` the vector bcorner(e)efore
     * passing it into this method.
     */
    void get_vertices_of_face(face_handle handle, vector<vertex_handle>& vertices_out) const;

    /**
     * @brief Adds the vertices surrounding the given face to the given set.
     *
     * @return The vertex-handles in counter-clockwise order. If a vertex does already exist in the set it will
     * be skipped. The vertex handles are written into the `vertices_out` set. This is done
     * to reduce the number of heap allocations if this method is called in
     * a loop. If you are not calling it in a loop or can't, for some reason,
     * take advantages of this method's signature, you can call the other
     * overload of this method which just returns a vector. Such convinient.
     *
     * Note: you probably should remember to `clear()` the set before
     * passing it into this method.
     */
    void get_vertices_of_face(face_handle handle, set<vertex_handle>& vertices_out) const;

    /**
     * @brief Get the two vertices of an edge.
     *
     * The order of the vertices is not specified
     */
    array<vertex_handle, 2> get_vertices_of_edge(edge_handle edge_h) const;

    /**
     * @brief Adds the two vertices of an edge to the given vector.
     *
     * The order of the vertices is not specified. The vertex handles are written
     * into the `vertices_out` vector. This is done
     * to reduce the number of heap allocations if this method is called in
     * a loop. If you are not calling it in a loop or can't, for some reason,
     * take advantages of this method's signature, you can call the other
     * overload of this method which just returns the vector. Such convinient.
     *
     * Note: you probably should remember to `clear()` the vector before
     * passing it into this method.
     */
    void get_vertices_of_edge(edge_handle edge_h, vector<vertex_handle>& vertices_out) const;

    /**
     * @brief Adds the two vertices of an edge to the given set.
     *
     * The order of the vertices is not specified. If a vertex does already exist in the set it will
     * be skipped. The vertex handles are written into the `vertices_out` set. This is done
     * to reduce the number of heap allocations if this method is called in
     * a loop. If you are not calling it in a loop or can't, for some reason,
     * take advantages of this method's signature, you can call the other
     * overload of this method which just returns a vector. Such convinient.
     *
     * Note: you probably should remember to `clear()` the set before
     * passing it into this method.
     */
    void get_vertices_of_edge(edge_handle edge_h, set<vertex_handle>& vertices_out) const;

    /**
     * @brief Get the two vertices of an half edge.
     *
     * The order of the vertices is not specified
     */
    array<vertex_handle, 2> get_vertices_of_half_edge(half_edge_handle edge_h) const;

    // ========================================================================
    // = Get faces
    // ========================================================================

    /**
     * @brief Get the two faces of an edge.
     *
     * The order of the faces is not specified
     */
    array<optional_face_handle, 2> get_faces_of_edge(edge_handle edge_h) const;

    /**
     * @brief Get the faces of a vertex.
     *
     * The handles of the faces around the given vertex will be in clockwise order.
     */
    vector<face_handle> get_faces_of_vertex(vertex_handle vh) const;

    /**
     * @brief Get a list of faces around the given vertex in clockwise order.
     *
     * The face handles are written into the `faces_out` vector. This is done
     * to reduce the number of heap allocations if this method is called in
     * a loop. If you are not calling it in a loop or can't, for some reason,
     * take advantages of this method's signature, you can call the other
     * overload of this method which just returns the vector. Such convinient.
     *
     * Note: you probably should remember to `clear()` the vector before
     * passing it into this method.
     *
     * @param faces_out The handles of the faces around `vh` will be written
     *                 into this vector in clockwise order.
     */
    void get_faces_of_vertex(vertex_handle vh, vector<face_handle>& faces_out) const;

    /**
     * @brief Get the face of a half edge.
     */
    optional_face_handle get_face_of_half_edge(half_edge_handle edge_h) const;

    /**
     * @brief Get face handles of the neighbours of the requested face.
     *
     * The face handles are written into the `faces_out` vector. This is done
     * to reduce the number of heap allocations if this method is called in
     * a loop. If you are not calling it in a loop or can't, for some reason,
     * take advantages of this method's signature, you can call the other
     * overload of this method which just returns the vector. Such convinient.
     *
     * Note: you probably should remember to `clear()` the vector before
     * passing it into this method.
     *
     * @param faces_out The face-handles of the neighbours of `handle` will be
     *                 written into this vector in counter-clockwise order.
     *                 There are at most four neighbours of a face, so this
     *                 method will push 0, 1, 2 or 3 handles to `faces_out`.
     */
    void get_neighbours_of_face(face_handle handle, vector<face_handle>& faces_out) const;

    /**
     * @brief Get face handles of the neighbours of the requested face.
     *
     * This method is implemented using the method
     * `get_neighbours_of_face(face_handle, vector<face_handle>&)`. If you are
     * calling this method in a loop, you should probably call the more manual
     * method (with the out vector) to avoid useless heap allocations.
     *
     * @return The face-handles of the neighbours in counter-clockwise order.
     */
    vector<face_handle> get_neighbours_of_face(face_handle handle) const;

    /**
     * @brief If all vertices are part of one face, this face is returned. None
     *        otherwise.
     *
     * The vertices don't have to be in a specific order. In particular, this
     * method will find a face regardless of whether the vertices are given in
     * clockwise or counter-clockwise order.
     */
    optional_face_handle get_face_between(const vector<vertex_handle>& handles) const;

    // ========================================================================
    // = Get edges
    // ========================================================================

    /**
     * @brief Returns true, if this half edge lies on the border of the mesh, false otherwise.
     */
     bool is_border(half_edge_handle handle) const;

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
     * @brief Get a list of edges around the given vertex.
     *
     * This method is implemented using the method
     * `get_edges_of_vertex(vertex_handle, vector<edge_handle>&)`. If you are
     * calling this method in a loop, you should probably call the more manual
     * method (with the out vector) to avoid useless heap allocations.
     *
     * @return The edge-handles in clockwise order.
     */
    vector<edge_handle> get_edges_of_vertex(vertex_handle handle) const;

    /**
     * @brief Get a list of half edges around the given vertex. If the edges are in
     *        or outgoing is determined by the way parameter.
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
    void get_half_edges_of_vertex(vertex_handle handle,
                                  vector<half_edge_handle>& edges_out,
                                  edge_direction way = edge_direction::ingoing) const;

    /**
     * @brief Get a list of half edges around the given vertex. If the edges are in
     *        or outgoing is determined by the way parameter.
     *
     * This method is implemented using the method
     * `get_half_edges_of_vertex(vertex_handle, vector<edge_handle>&, direction)`. If you are
     * calling this method in a loop, you should probably call the more manual
     * method (with the out vector) to avoid useless heap allocations.
     *
     * @return The edge-handles in clockwise order.
     */
    vector<half_edge_handle> get_half_edges_of_vertex(vertex_handle handle, edge_direction way = edge_direction::ingoing) const;

    /**
     * @brief Get inner half edges of face in counter clockwise order
     */
    vector<half_edge_handle> get_half_edges_of_face(face_handle face_handle) const;

    /**
     * @brief Get the two half edges of an edge.
     */
    array<half_edge_handle, 2> get_half_edges_of_edge(edge_handle edge_h) const;

    /**
     * @brief If the two given vertices are connected by an edge, it is
     *        returned. None otherwise.
     */
    optional_edge_handle get_edge_between(vertex_handle ah, vertex_handle bh) const;

    /**
     * @brief If the two given vertices are connected by an edge, the half edge
     *        targeting `bh` is returned. None otherwise.
     */
    optional_half_edge_handle get_half_edge_between(vertex_handle ah, vertex_handle bh) const;

    /**
     * @brief If the two given faces are neighbours, the half edge between them (connectd to ah)
     *        is returned. None otherwise.
     */
    optional_half_edge_handle get_half_edge_between(face_handle ah, face_handle bh) const;

    // ========================================================================
    // = Iterator helper
    // ========================================================================
    tmesh_iterator_ptr<vertex_handle> vertices_begin() const;
    tmesh_iterator_ptr<vertex_handle> vertices_end() const;
    tmesh_iterator_ptr<face_handle> faces_begin() const;
    tmesh_iterator_ptr<face_handle> faces_end() const;
    tmesh_iterator_ptr<half_edge_handle> half_edges_begin() const;
    tmesh_iterator_ptr<half_edge_handle> half_edges_end() const;
    tmesh_iterator_ptr<edge_handle> edges_begin() const;
    tmesh_iterator_ptr<edge_handle> edges_end() const;

    /**
     * @brief Method for usage in range-based for-loops.
     *
     * Returns a simple proxy object that uses `faces_begin()` and `faces_end()`.
     */
    tmesh_face_iterator_proxy get_faces() const;

    /**
     * @brief Method for usage in range-based for-loops.
     *
     * Returns a simple proxy object that uses `half_edges_begin()` and `half_edges_end()`.
     */
    tmesh_half_edge_iterator_proxy get_half_edges() const;

    /**
     * @brief Method for usage in range-based for-loops.
     *
     * Returns a simple proxy object that uses `edges_begin()` and `edges_end()`.
     */
    tmesh_edge_iterator_proxy get_edges() const;

    /**
     * @brief Method for usage in range-based for-loops.
     *
     * Returns a simple proxy object that uses `vertices_begin()` and `vertices_end()`.
     */
    tmesh_vertex_iterator_proxy get_vertices() const;

private:
    stable_vector<half_edge_handle, half_edge> edges;
    stable_vector<face_handle, face> faces;
    stable_vector<vertex_handle, vertex> vertices;

    // ========================================================================
    // = Private helper methods
    // ========================================================================
    half_edge& get_e(half_edge_handle handle);
    const half_edge& get_e(half_edge_handle handle) const;
    face& get_f(face_handle handle);
    const face& get_f(face_handle handle) const;
    vertex& get_v(vertex_handle handle);
    const vertex& get_v(vertex_handle handle) const;

    /**
     * @brief Attempts to find an edge between the given vertices and, if none
     *        is found, creates a new edge with `add_edge_pair()`
     *
     * @return The half edge from `from_h` to `to_h`
     */
    half_edge_handle find_or_create_edge_between(new_face_vertex from_h, new_face_vertex to_h);

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
    pair<half_edge_handle, half_edge_handle> add_edge_pair(new_face_vertex v1h, new_face_vertex v2h);

    /**
     * @brief Converts a half edge handle to a full edge handle
     *
     * @return The handle with the smaller index of the given half edge and its twin.
     */
    edge_handle half_to_full_edge_handle(half_edge_handle handle) const;

    /**
     * @brief Circulates around the vertex `vh`, calling the `visitor` for each
     *        edge of the vertex. If the in or outgoing edges are visited is
     *        determined by the way parameter.
     *
     * The edges are visited in clockwise order. The iteration stops if all
     * edges were visited once or if the visitor returns `false`. It has to
     * return `true` to keep circulating. If the vertex has no outgoing edge,
     * this method does nothing.
     */
    template <typename visitor_t>
    void circulate_around_vertex(vertex_handle vh, visitor_t visitor, edge_direction way = edge_direction::ingoing) const;

    /**
     * @brief Circulates clockwise around the vertex, calling the
     *        `visitor` for each edge of the vertex. If the in or outgoing edges are visited is
     *        determined by the way parameter.
     *
     * IMPORTANT: The start_edge has to match the direction of the way parameter!
     */
    template <typename visitor_t>
    void circulate_around_vertex(half_edge_handle start_edge_h, visitor_t visitor, edge_direction way = edge_direction::ingoing) const;

    /**
     * @brief Circulates over the inner edges of the given face `fh` in counter clockwise order,
     *        calling the `visitor` for each inner edge of the face.
     *
     * The edges are visited in clockwise order. The iteration stops if all inner
     * edges were visited once or if the visitor returns `false`. It has to
     * return `true` to keep circulating.
     */
    template <typename visitor_t>
    void circulate_in_face(face_handle fh, visitor_t visitor) const;

    /**
     * @brief Circulates over the inner edges of the face `start_edge_h.face`, calling the
     *        `visitor` for each inner edge of the face.
     *
     * This works exactly as the other overload, but specifically starts at the
     * edge `start_edge_h` instead of `face.edge`.
     */
    template <typename visitor_t>
    void circulate_in_face(half_edge_handle start_edge_h, visitor_t visitor) const;

    /**
     * @brief Iterates over all edges of one vertex, returning the
     *        first edge that satisfies the given predicate. If the
     *        in or outgoing edges are visited is determined by the way parameter.
     *
     * @return Returns None if `v` does not have an outgoing edge or if no
     *         edge in the circle satisfies the predicate.
     */
    template <typename pred_t>
    optional_half_edge_handle
    find_edge_around_vertex(vertex_handle vh, pred_t pred, edge_direction way = edge_direction::ingoing) const;

    /**
     * @brief Iterates over all edges of the vertex, returning the first edge that
     *        satisfies the given predicate. If the in or outgoing edges are visited is
     *        determined by the way parameter.
     *
     * IMPORTANT: The start_edge has to match the direction of the way parameter!
     *
     * @return Returns None if no edge in the circle satisfies the predicate.
     */
    template <typename pred_t>
    optional_half_edge_handle find_edge_around_vertex(half_edge_handle start_edge_h, pred_t pred, edge_direction way = edge_direction::ingoing) const;

    // ========================================================================
    // = Friends
    // ========================================================================
    friend class tmesh_edge_iterator;
};

}

#include "tmesh.tcc"

#endif //TSL_TMESH_HPP
