#include <tuple>
#include <memory>
#include <algorithm>
#include <optional>

#include "tsl/geometry/tmesh/tmesh.hpp"
#include "tsl/geometry/tmesh/iterator.hpp"
#include "tsl/algorithm/get_vertices.hpp"

using std::make_unique;
using std::min;
using std::get;
using std::make_tuple;
using std::tuple;
using std::nullopt;
using std::find;
using std::make_pair;
using std::find_if;
using std::distance;

namespace tsl {

// ========================================================================
// = Modifier
// ========================================================================
vertex_handle tmesh::add_vertex(vec3 pos) {
    vertex v;
    v.pos = pos;
    return vertices.push(v);
}

bool tmesh::is_face_insertion_valid(const vector<new_face_vertex>& new_vertices) const {
    // TODO: This implementation is incomplete! Check it for edge cases.
    // TODO: This does not detect non-manifold edges!

    // We are working with a quad mesh, so a face needs at least 4 vertices!
    if (new_vertices.size() < 4) {
        return false;
    }

    // Extract the handles from the given `new_face_vertex` and check corner and knot values
    vector<vertex_handle> new_vertex_handles;
    {
        new_vertex_handles.reserve(new_vertices.size());
        size_t num_corners = 0;
        for (const auto& new_vertex: new_vertices) {
            new_vertex_handles.push_back(new_vertex.handle);

            // Check knot values > 0
            if (new_vertex.knot <= 0) {
                return false;
            }
            if (new_vertex.corner) {
                num_corners += 1;
            }
        }
        if (num_corners != 4) {
            return false;
        }
    }

    // If there is already a face, we can't add another one
    if (get_face_between(new_vertex_handles)) {
        return false;
    }

    // Next we check that each vertex is a boundary one (it has at least one
    // boundary edge or no edges at all). We really need this property;
    // otherwise we would create non-manifold vertices and make the mesh non-
    // orientable.
    {
        vector<edge_handle> temp_edges;
        for (const auto& vh: new_vertex_handles) {
            temp_edges.clear();
            get_edges_of_vertex(vh, temp_edges);

            // No edges at all are fine too.
            if (temp_edges.empty()) {
                continue;
            }

            // But if there are some, we need at least one boundary one.
            auto boundary_edge_it = find_if(temp_edges.begin(), temp_edges.end(), [&, this](auto eh) {
                return this->num_adjacent_faces(eh) < 2;
            });
            if (boundary_edge_it == temp_edges.end()) {
                return false;
            }
        }
    }

    return true;
}

bool tmesh::is_face_insertion_valid(const array<vertex_handle, 4>& new_vertices) const {
    return is_face_insertion_valid({
        new_face_vertex(new_vertices[0]),
        new_face_vertex(new_vertices[1]),
        new_face_vertex(new_vertices[2]),
        new_face_vertex(new_vertices[3])
    });
}

face_handle tmesh::add_face(const vector<new_face_vertex>& new_vertices) {
    // TODO: This implementation is incomplete! Check it for edge cases.
    // TODO: Sometimes this method get's stuck in an endless loop -> investigate!

    if (!is_face_insertion_valid(new_vertices)) {
        panic("attempting to add a face which cannot be added!");
    }

    // We have to create the following objects and set their attributes:
    // | type              | attribute | created/set in step |
    // |-------------------|-----------|---------------------|
    // | half_edge (inner) | -         | 1                   |
    // | half_edge (inner) | face      | 2                   |
    // | half_edge (inner) | target    | 1                   |
    // | half_edge (inner) | next      | 4                   |
    // | half_edge (inner) | prev      | 4                   |
    // | half_edge (inner) | corner    | 2                   |
    // | half_edge (inner) | knot      | 2                   |
    // | half_edge (outer) | -         | 1                   |
    // | half_edge (outer) | face      | no need to be set   |
    // | half_edge (outer) | target    | 1                   |
    // | half_edge (outer) | next      | 3                   |
    // | half_edge (outer) | prev      | 3                   |
    // | half_edge (outer) | corner    | no need to be set   |
    // | half_edge (outer) | knot      | no need to be set   |
    // | face              | -         | 2                   |
    // | face              | edge      | 2CHECK BASED CORNER |
    // | vertex            | outgoing  | 2                   |

    // =======================================================================
    // = Create broken edges (Step 1)
    // =======================================================================
    // Handles for the inner edges of the face. The edges represented by those
    // handles do not contain a valid `next` and `prev` pointer yet.
    vector<half_edge_handle> inner_handles;
    {
        inner_handles.reserve(new_vertices.size());
        for (size_t i = 1; i < new_vertices.size(); ++i) {
            auto inner_h = find_or_create_edge_between(new_vertices[i - 1], new_vertices[i]);
            inner_handles.push_back(inner_h);
        }
        auto inner_h = find_or_create_edge_between(new_vertices.back(), new_vertices.front());
        inner_handles.push_back(inner_h);
    }

    // =======================================================================
    // = Create face (Step 2)
    // =======================================================================
    face_handle new_face_h = faces.next_handle();
    {
        // Get half edge based at a corner
        auto found = find_if(new_vertices.begin(), new_vertices.end(), [&, this](const auto& new_vertex) {
            return new_vertex.corner;
        });
        if (found == new_vertices.end()) {
            panic("A face without four corners cannot be added to the T-Mesh! (Check your corner values.)");
        }
        auto index = distance(new_vertices.begin(), found);
        face f(inner_handles[index]);
        faces.push(f);
    }

    // Set face handle of edges and get handles for outer edges
    vector<half_edge_handle> outer_handles;
    {
        outer_handles.reserve(new_vertices.size());
        for (size_t i = 0; i < inner_handles.size(); ++i) {
            auto vertex_index = i + 1;
            if (i == inner_handles.size() - 1) {
                vertex_index = 0;
            }
            const auto& target = new_vertices[vertex_index];

            auto e_inner_h = inner_handles[i];
            auto& e_inner = get_e(e_inner_h);
            e_inner.face = optional_face_handle(new_face_h);
            e_inner.knot = target.knot;
            e_inner.corner = target.corner;
            outer_handles.push_back(get_twin(e_inner_h));
        }
    }

    // =======================================================================
    // = Fix next and prev handles (Step 3)
    // =======================================================================
    // Fixing the `next` and `prev` handles is the most difficult part of this method. In
    // order to tackle it we deal with each corner of this face on its own.
    // For each corner we look at the corner-vertex and the in-going and
    // out-going edge (both edges are on the outside of this face!).
    vector<tuple<half_edge_handle, vertex_handle, half_edge_handle>> corners;
    corners.reserve(new_vertices.size());
    corners.emplace_back(outer_handles.front(), new_vertices.front().handle, outer_handles.back());
    for (size_t i = 1; i < new_vertices.size(); ++i)
    {
        corners.emplace_back(outer_handles[i], new_vertices[i].handle, outer_handles[i - 1]);
    }

    // TODO: check for more special cases due to quad faces
    for (const auto& corner: corners) {
        auto e_in_h = get<0>(corner);
        auto e_in_twin_h = get_twin(e_in_h);
        auto vh = get<1>(corner);
        auto e_out_h = get<2>(corner);
        auto e_out_twin_h = get_twin(e_out_h);

        auto& e_in = get_e(e_in_h);
        auto& v = get_v(vh);
        auto& e_out = get_e(e_out_h);

        // For each corner, we have four different cases, depending on whether
        // or not the in/outgoing edges are already part of a face.
        //
        // --> Case (A): neither edge is part of a face (both edges are new)
        if (!e_in.face && !e_out.face)
        {
            // We need to handle the special case of `v` not having an
            // outgoing edge.
            if (v.outgoing)
            {
                // First we need to find two boundary edges adjacent to `v`. One
                // ending at `v`, called e_end, and one starting at `v`, called
                // e_start. Note that `e_end.next == e_start`.
                //
                // Since we already know that `v.outgoing` does exist,
                // `find_edge_around_vertex` returning `none` means that `v` does
                // not have an adjacent boundary edge.
                //
                // A classical HEM can't deal with these kind of so called
                // non-manifold meshes. We don't expect non-manifold
                // meshes, so for now we just panic. Having this restriction to
                // manifold meshes is probably not too bad.
                auto e_end_h = find_edge_around_vertex(vh, [&, this](auto edge_h)
                {
                    return !get_e(edge_h).face;
                }, edge_direction::ingoing).expect("a non-manifold part in the mesh has been found");

                auto e_start_h = get_e(e_end_h).next;
                auto& e_start = get_e(e_start_h);

                e_in.next = e_start_h;
                e_start.prev = e_in_h;

                get_e(e_end_h).next = e_out_h;
                e_out.prev = e_end_h;
            } else
            {
                // `e_in` and `e_out` are the only edges of the vertex.
                e_in.next = e_out_h;
                e_out.prev = e_in_h;
            }
        }
            // --> Case (B): only the ingoing edge is part of a face
        else if (e_in.face && !e_out.face)
        {
            // We know that `v` has at least two outgoing edges (since
            // there is a face adjacent to it).
            //
            // We have to find the edge which `next` handle pointed to the
            // outer edge of the one face we are adjacent to (called
            // `old_next`. This is an inner edge of our face.
            auto eh = get_e(e_in_twin_h).prev;

            get_e(eh).next = e_out_h;
            e_out.prev = eh;
        }
            // --> Case (C): only the outgoing edge is part of a face
        else if (!e_in.face && e_out.face)
        {
            // This is correct, because the old next pointer are still present!
            auto old_out_h = get_e(e_out_twin_h).next;
            e_in.next = old_out_h;
            get_e(old_out_h).prev = e_in_h;
        }
            // --> Case (D): both edges are already part of another face
        else if (e_in.face && e_out.face)
        {
            // Here, two fan blades around `v` will be connected. Both blades
            // need to be in the right order for this to work. The order is
            // given by the `next` and `prev` handles of the edges with the target `v`.
            // By following those handles (and `twin` handles), we can
            // circulate around `v`.
            //
            // When circulating, both fan blades need to be next to each other.
            // If that's not the case, we need to fix a few `next` and `prev` handles. Not
            // being in the right order is caused by case (A), but it can't be
            // avoided there. Only when connecting the blades here, we can know
            // how to create the `next` and `prev` circle.
            if (get_e(e_out_twin_h).next != e_in_twin_h)
            {
                // Here we need to conceptually delete one fan blade from the
                // `next` and `prev` circle around `v` and re-insert it into the right
                // position. We choose to "move" the fan blade starting with
                // `e_in`.
                //
                // We search the edge that points to
                // `e_in.twin`.
                auto inactive_blade_end_h = get_e(e_in_twin_h).prev;

                // Instead of pointing to `e_in.twin`, it needs to "skip" the
                // `e_in` blade and point to the blade afterwards. So we need to
                // find the end of the `e_in` blade. Its next handle is the one
                // `inactive_blade_end` needs to point to.
                auto e_in_blade_end_h = find_edge_around_vertex(
                    e_in_h,
                    [&, this](auto edge_h)
                    {
                        return !get_e(edge_h).face;
                    },
                    edge_direction::ingoing
                ).unwrap();

                // We can finally set the next and prev pointer to skip the `e_in`
                // blade. After this line, circulating around `v` will work
                // but skip the whole `e_in` blade.
                get_e(inactive_blade_end_h).next = get_e(e_in_blade_end_h).next;
                get_e(get_e(e_in_blade_end_h).next).prev = inactive_blade_end_h;

                // Now we need to re-insert it again. Fortunately, this is
                // easier. After this line, the circle is broken, but it will
                // be repaired by repairing the `next` and `prev` handles within the face
                // later.
                get_e(e_in_blade_end_h).next = get_e(e_out_twin_h).next;
                get_e(get_e(e_out_twin_h).next).prev = e_in_blade_end_h;
            }
        }
    }

    // =======================================================================
    // = Set `next` and `prev` handle of inner edges (Step 4)
    // =======================================================================
    // This is an easy step, but we can't
    // do it earlier, since the old `next` and `prev` handles are required by the
    // previous "fix next and prev handles" step.
    auto& e_inner_first = get_e(inner_handles.front());
    e_inner_first.next = inner_handles[1];
    e_inner_first.prev = inner_handles.back();
    for (size_t i = 1; i < (inner_handles.size() - 1); ++i)
    {
        auto& e_inner = get_e(inner_handles[i]);
        e_inner.next = inner_handles[i + 1];
        e_inner.prev = inner_handles[i - 1];
    }
    auto& e_inner_last = get_e(inner_handles.back());
    e_inner_last.next = inner_handles.front();
    e_inner_last.prev = inner_handles[inner_handles.size() - 2];

    // =======================================================================
    // = Set outgoing handles if not set yet (Step 5)
    // =======================================================================
    // Set outgoing-handles if they are not set yet.
    for (size_t i = 0; i < new_vertices.size(); ++i)
    {
        auto& v = get_v(new_vertices[i].handle);
        auto e_inner_h = inner_handles[i];
        if (!v.outgoing)
        {
            v.outgoing = optional_half_edge_handle(e_inner_h);
        }
    }

    return new_face_h;
}

face_handle tmesh::add_face(const array<vertex_handle, 4>& new_vertices) {
    return add_face({
        new_face_vertex(new_vertices[0]),
        new_face_vertex(new_vertices[1]),
        new_face_vertex(new_vertices[2]),
        new_face_vertex(new_vertices[3])
    });
}

bool tmesh::remove_edge(edge_handle handle) {
    // TODO: This implementation is incomplete! Check it for edge cases.
    auto [half_edge_1_h, half_edge_2_h] = get_half_edges_of_edge(handle);
    const auto& half_edge_1 = get_e(half_edge_1_h);
    const auto& half_edge_2 = get_e(half_edge_2_h);

    // TODO: Check, if T-Mesh constraints are fulfilled after edge is removed!

    // We may not delete edges near extraordinary vertices
    {
        if (is_extraordinary(half_edge_1.target) || is_extraordinary(half_edge_2.target)) {
            return false;
        }

        // find extraordinary vertices around one vertex
        auto vertices1 = get_extraordinary_vertices_in_regular_rings_around_vertex(*this, half_edge_1.target, 3);
        bool found = false;

        // if one extraordinary vertex is near both vertices of the edge, the edge is too close to that vertex
        // and we are not allowed to remove it
        visit_regular_rings(*this, half_edge_2.target, 3, [this, &vertices1, &found](auto handle, auto ring) {
            if (is_extraordinary(handle)) {
                auto in_vertices = find(vertices1.begin(), vertices1.end(), handle);
                if (vertices1.end() != in_vertices) {
                    found = true;
                    return false;
                }
            }
            return true;
        });

        if (found) {
            return false;
        }
    }

    // we cannot remove edges pointing to vertices which have 2 or less edges connected
    if (get_valence(half_edge_1.target) < 3 || get_valence(half_edge_2.target) < 3) {
        return false;
    }

    // TODO: check cylce and consistency conditions, if edge would be removed!

    // we cannot remove border edges
    if (!half_edge_1.face || !half_edge_2.face) {
        return false;
    }
    auto face1_h = half_edge_1.face.unwrap();
    auto face2_h = half_edge_2.face.unwrap();

    // get vertices
    auto& vertex_1 = get_v(half_edge_1.target);
    auto& vertex_2 = get_v(half_edge_2.target);

    // we have to fix:
    // - prev
    // - next
    // - out (of vertex)
    // - edge (of face)
    // - face (of inner edges of face 1)
    // - corner

    // TODO: special case: valence == 3
    if (get_valence(half_edge_1.target)  == 3 || get_valence(half_edge_2.target)  == 3) {
        // Check for T-Joints which could be removed
        // TODO: when a edge inc to a t-joint is removed, it removes the vertex as well
        return false;
    }

    // the face of half edge 1 is going to be deleted by this operation, store all inner edge handles, to fix their
    // face pointer
    auto inner_edges_of_face1 = get_half_edges_of_face(face1_h);

    // fix prev and next
    auto& next_1 = get_e(half_edge_1.next);
    auto& prev_1 = get_e(half_edge_1.prev);
    auto& next_2 = get_e(half_edge_2.next);
    auto& prev_2 = get_e(half_edge_2.prev);

    next_1.prev = half_edge_2.prev;
    next_2.prev = half_edge_1.prev;
    prev_1.next = half_edge_2.next;
    prev_2.next = half_edge_1.next;

    // fix corner
    prev_1.corner = false;
    prev_2.corner = false;

    // fix out (of vertex)
    if (vertex_1.outgoing.unwrap() == half_edge_2_h) {
        vertex_1.outgoing = optional_half_edge_handle(half_edge_1.next);
    }
    if (vertex_2.outgoing.unwrap() == half_edge_1_h) {
        vertex_2.outgoing = optional_half_edge_handle(half_edge_2.next);
    }

    // fix edge (of face 2)
    auto& face2 = get_f(face2_h);
    if (!*from_corner(face2.edge)) {
        // Find a half edge which is based at a corner
        optional_half_edge_handle based_at_corner;
        circulate_in_face(half_edge_2.prev, [&based_at_corner, this](auto handle) {
            if (*from_corner(handle)) {
                based_at_corner = optional_half_edge_handle(handle);
                return false;
            }
            return true;
        });

        face2.edge = based_at_corner.expect("No edge based at a corner found in face!");
    }

    // fix face (of inner edges of face 1)
    for (const auto& eh: inner_edges_of_face1) {
        auto& half_edge = get_e(eh);
        half_edge.face = optional_face_handle(face2_h);
    }

    // actually delete the edge and face 1
    edges.erase(half_edge_1_h);
    edges.erase(half_edge_2_h);
    faces.erase(face1_h);

    return true;
}

// ========================================================================
// = Get numbers
// ========================================================================
size_t tmesh::num_vertices() const
{
    return vertices.num_used();
}

size_t tmesh::num_faces() const
{
    return faces.num_used();
}

size_t tmesh::num_edges() const
{
    return edges.num_used() / 2;
}

size_t tmesh::num_half_edges() const
{
    return edges.num_used();
}

uint8_t tmesh::num_adjacent_faces(edge_handle handle) const
{
    auto faces_of_edge = get_faces_of_edge(handle);
    return static_cast<uint8_t>((faces_of_edge[0] ? 1 : 0) + (faces_of_edge[1] ? 1 : 0));
}

size_t tmesh::get_valence(vertex_handle handle) const {
    index valence = 0;
    circulate_around_vertex(handle, [&, this](auto ingoing_edge_h)
    {
        valence += 1;
        return true;
    });
    return valence;
}

size_t tmesh::get_extended_valence(vertex_handle handle) const {
    uint32_t valence = 0;
    for (const auto& e: get_half_edges_of_vertex(handle, edge_direction::ingoing)) {
        valence += 1;
        if (corner(e) && !*corner(e)) {
            valence += 1;
        }
    }
    return valence;
}

bool tmesh::is_extraordinary(vertex_handle handle) const {
    return get_extended_valence(handle) != 4;
}

// ========================================================================
// = Get attributes
// ========================================================================
vec3 tmesh::get_vertex_position(vertex_handle handle) const
{
    return get_v(handle).pos;
}

vec3& tmesh::get_vertex_position(vertex_handle handle)
{
    return get_v(handle).pos;
}

optional<double> tmesh::get_knot_interval(half_edge_handle handle) const {
    return get_e(handle).knot;
}

optional<bool> tmesh::corner(half_edge_handle handle) const {
    return get_e(handle).corner;
}

optional<bool> tmesh::from_corner(half_edge_handle handle) const {
    return get_e(get_e(handle).prev).corner;
}

optional<double> tmesh::get_knot_factor(half_edge_handle handle) const {
    auto knot_interval = get_knot_interval(handle);
    auto knot_interval_twin = get_knot_interval(get_twin(handle));
    if (knot_interval && knot_interval_twin) {
        return *knot_interval / *knot_interval_twin;
    }
    return nullopt;
}

// ========================================================================
// = Follow pointer
// ========================================================================
half_edge_handle tmesh::get_twin(half_edge_handle handle) const {
    return half_edge_handle(handle.get_idx() ^ 1);
}

half_edge_handle tmesh::get_prev(half_edge_handle handle) const
{
    return get_e(handle).prev;
}

half_edge_handle tmesh::get_next(half_edge_handle handle) const
{
    return get_e(handle).next;
}

vertex_handle tmesh::get_target(half_edge_handle handle) const
{
    return get_e(handle).target;
}

optional_half_edge_handle tmesh::get_out(vertex_handle handle) const {
    return get_v(handle).outgoing;
}

half_edge_handle tmesh::get_edge(face_handle handle) const {
    return get_f(handle).edge;
}

// ========================================================================
// = Get vertices
// ========================================================================
vector<vertex_handle> tmesh::get_vertices_of_face(face_handle handle) const
{
    vector<vertex_handle> vertices_out;
    vertices_out.reserve(4);
    get_vertices_of_face(handle, vertices_out);
    return vertices_out;
}

void tmesh::get_vertices_of_face(face_handle handle, vector<vertex_handle>& vertices_out) const
{
    circulate_in_face(handle, [&vertices_out, this](auto eh)
    {
        vertices_out.push_back(get_e(eh).target);
        return true;
    });
}

void tmesh::get_vertices_of_face(face_handle handle, set<vertex_handle>& vertices_out) const
{
    circulate_in_face(handle, [&vertices_out, this](auto eh)
    {
        vertices_out.insert(get_e(eh).target);
        return true;
    });
}

array<vertex_handle, 2> tmesh::get_vertices_of_edge(edge_handle edge_h) const
{
    auto one_edge_h = half_edge_handle::one_half_of(edge_h);
    const auto& one_edge = get_e(one_edge_h);
    return {one_edge.target, get_e(get_twin(one_edge_h)).target};
}

void tmesh::get_vertices_of_edge(edge_handle edge_h, vector<vertex_handle>& vertices_out) const {
    auto one_edge_h = half_edge_handle::one_half_of(edge_h);
    const auto& one_edge = get_e(one_edge_h);
    vertices_out.push_back(one_edge.target);
    vertices_out.push_back(get_e(get_twin(one_edge_h)).target);
}

void tmesh::get_vertices_of_edge(edge_handle edge_h, set<vertex_handle>& vertices_out) const {
    auto one_edge_h = half_edge_handle::one_half_of(edge_h);
    const auto& one_edge = get_e(one_edge_h);
    vertices_out.insert(one_edge.target);
    vertices_out.insert(get_e(get_twin(one_edge_h)).target);
}

array<vertex_handle, 2> tmesh::get_vertices_of_half_edge(half_edge_handle edge_h) const
{
    const auto& one_edge = get_e(edge_h);
    return {one_edge.target, get_e(get_twin(edge_h)).target};
}

// ========================================================================
// = Get faces
// ========================================================================
array<optional_face_handle, 2> tmesh::get_faces_of_edge(edge_handle edge_h) const
{
    auto one_edge_h = half_edge_handle::one_half_of(edge_h);
    const auto& one_edge = get_e(one_edge_h);
    return {one_edge.face, get_e(get_twin(one_edge_h)).face};
}

vector<face_handle> tmesh::get_faces_of_vertex(vertex_handle vh) const {
    vector<face_handle> out;
    get_faces_of_vertex(vh, out);
    return out;
}

void tmesh::get_faces_of_vertex(vertex_handle vh, vector<face_handle>& faces_out) const {
    circulate_around_vertex(vh, [&faces_out, this](auto eh)
    {
        const auto& e = get_e(eh);
        if (e.face) {
            faces_out.push_back(e.face.unwrap());
        }
        return true;
    });
}

optional_face_handle tmesh::get_face_of_half_edge(half_edge_handle edge_h) const
{
    const auto& edge = get_e(edge_h);
    return edge.face;
}

void tmesh::get_neighbours_of_face(face_handle handle, vector<face_handle>& faces_out) const
{
    auto inner_edges = get_half_edges_of_face(handle);
    for (const auto& eh: inner_edges)
    {
        const auto& twin = get_e(get_twin(eh));
        if (twin.face)
        {
            faces_out.push_back(twin.face.unwrap());
        }
    }
}

vector<face_handle> tmesh::get_neighbours_of_face(face_handle handle) const
{
    vector<face_handle> out;
    get_neighbours_of_face(handle, out);
    return out;
}

optional_face_handle tmesh::get_face_between(const vector<vertex_handle>& handles) const {
    // TODO: This is more compilcated! what if a face of v0, v1, v2, v3 exists and we try to create a face
    //       with v0-v6? Search for more edge cases!
    // TODO: This implementation is incomplete! Check it for edge cases.
    if (handles.size() < 4) {
        panic("too few vertex handles! A face in a quad mesh needs at least four vertices!");
    }

    // Start with one edge
    const auto ab_edge_h = get_edge_between(handles[0], handles[1]);

    // If the edge already doesn't exist, there won't be a face either.
    if (!ab_edge_h) {
        return optional_face_handle();
    }

    // The two faces of the edge. One of these faces should contain the vertex
    // `c` or there is just no face between the given vertices.
    const auto faces_of_edge = get_faces_of_edge(ab_edge_h.unwrap());

    // Find the face which contains vertex `c`.
    auto face_it = find_if(faces_of_edge.begin(), faces_of_edge.end(), [&, this](auto maybe_face_h) {
        if (!maybe_face_h) {
            return false;
        }
        const auto vertices_of_face = this->get_vertices_of_face(maybe_face_h.unwrap());
        return find(vertices_of_face.begin(), vertices_of_face.end(), handles[2]) != vertices_of_face.end();
    });

    return face_it != faces_of_edge.end() ? *face_it : optional_face_handle();
}

// ========================================================================
// = Get edges
// ========================================================================
bool tmesh::is_border(half_edge_handle handle) const {
    const auto& he = get_e(handle);
    return !he.face;
}

void tmesh::get_edges_of_vertex(
    vertex_handle handle,
    vector<edge_handle>& edges_out
) const
{
    circulate_around_vertex(handle, [&edges_out, this](auto eh)
    {
        edges_out.push_back(half_to_full_edge_handle(eh));
        return true;
    });
}

vector<edge_handle> tmesh::get_edges_of_vertex(vertex_handle handle) const
{
    vector<edge_handle> out;
    get_edges_of_vertex(handle, out);
    return out;
}

void tmesh::get_half_edges_of_vertex(
    vertex_handle handle,
    vector<half_edge_handle>& edges_out,
    edge_direction way
) const
{
    circulate_around_vertex(handle, [&edges_out, this](auto eh)
    {
        edges_out.push_back(eh);
        return true;
    }, way);
}

vector<half_edge_handle> tmesh::get_half_edges_of_vertex(vertex_handle handle, edge_direction way) const
{
    vector<half_edge_handle> out;
    get_half_edges_of_vertex(handle, out, way);
    return out;
}

vector<half_edge_handle> tmesh::get_half_edges_of_face(face_handle face_handle) const
{
    vector<half_edge_handle> edges_out;
    edges_out.reserve(4);
    circulate_in_face(face_handle, [&edges_out, this](auto eh)
    {
        edges_out.push_back(eh);
        return true;
    });
    return edges_out;
}

array<half_edge_handle, 2> tmesh::get_half_edges_of_edge(edge_handle edge_h) const
{
    auto heh = half_edge_handle::one_half_of(edge_h);
    return {heh, get_twin(heh)};
}

optional_edge_handle tmesh::get_edge_between(vertex_handle ah, vertex_handle bh) const
{
    // Go through all edges of vertex `a` until we find an edge that is also
    // connected to vertex `b`.
    for (const auto& eh: get_edges_of_vertex(ah))
    {
        auto endpoints = get_vertices_of_edge(eh);
        if (endpoints[0] == bh || endpoints[1] == bh)
        {
            return optional_edge_handle(eh);
        }
    }
    return optional_edge_handle();
}

optional_half_edge_handle tmesh::get_half_edge_between(vertex_handle ah, vertex_handle bh) const
{
    // Go through all edges of vertex `a` until we find an edge that is also
    // connected to vertex `b`.
    return find_edge_around_vertex(ah, [&, this](auto current_edge_h) {
        return get_e(current_edge_h).target == bh;
    }, edge_direction::outgoing);
}

optional_half_edge_handle tmesh::get_half_edge_between(face_handle ah, face_handle bh) const
{
    optional_half_edge_handle out;
    circulate_in_face(ah, [&, this](auto eh)
    {
        const auto& twin = get_e(get_twin(eh));
        if (twin.face && twin.face.unwrap() == bh)
        {
            out = optional_half_edge_handle(eh);
            return false;
        }
        return true;
    });

    return out;
}

// ========================================================================
// = Iterator helper
// ========================================================================
tmesh_iterator_ptr<vertex_handle> tmesh::vertices_begin() const
{
    return tmesh_iterator_ptr<vertex_handle>(
        make_unique<tmesh_fev_iterator<vertex_handle, vertex>>(vertices.begin())
    );
}

tmesh_iterator_ptr<vertex_handle> tmesh::vertices_end() const
{
    return tmesh_iterator_ptr<vertex_handle>(
        make_unique<tmesh_fev_iterator<vertex_handle, vertex>>(vertices.end())
    );
}

tmesh_iterator_ptr<face_handle> tmesh::faces_begin() const
{
    return tmesh_iterator_ptr<face_handle>(
        make_unique<tmesh_fev_iterator<face_handle, face>>(faces.begin())
    );
}

tmesh_iterator_ptr<face_handle> tmesh::faces_end() const
{
    return tmesh_iterator_ptr<face_handle>(
        make_unique<tmesh_fev_iterator<face_handle, face>>(faces.end())
    );
}

tmesh_iterator_ptr<half_edge_handle> tmesh::half_edges_begin() const
{
    return tmesh_iterator_ptr<half_edge_handle>(
        make_unique<tmesh_fev_iterator<half_edge_handle, half_edge>>(edges.begin())
    );
}

tmesh_iterator_ptr<half_edge_handle> tmesh::half_edges_end() const
{
    return tmesh_iterator_ptr<half_edge_handle>(
        make_unique<tmesh_fev_iterator<half_edge_handle, half_edge>>(edges.end())
    );
}

tmesh_iterator_ptr<edge_handle> tmesh::edges_begin() const
{
    return tmesh_iterator_ptr<edge_handle>(
        make_unique<tmesh_edge_iterator>(edges.begin(), *this)
    );
}

tmesh_iterator_ptr<edge_handle> tmesh::edges_end() const
{
    return tmesh_iterator_ptr<edge_handle>(
        make_unique<tmesh_edge_iterator>(edges.end(), *this)
    );
}

tmesh_face_iterator_proxy tmesh::get_faces() const
{
    return tmesh_face_iterator_proxy(*this);
}

tmesh_half_edge_iterator_proxy tmesh::get_half_edges() const
{
    return tmesh_half_edge_iterator_proxy(*this);
}

tmesh_edge_iterator_proxy tmesh::get_edges() const
{
    return tmesh_edge_iterator_proxy(*this);
}

tmesh_vertex_iterator_proxy tmesh::get_vertices() const
{
    return tmesh_vertex_iterator_proxy(*this);
}

// ========================================================================
// = Private helper methods
// ========================================================================
half_edge& tmesh::get_e(half_edge_handle handle)
{
    return edges[handle];
}

const half_edge& tmesh::get_e(half_edge_handle handle) const
{
    return edges[handle];
}

face& tmesh::get_f(face_handle handle)
{
    return faces[handle];
}

const face& tmesh::get_f(face_handle handle) const
{
    return faces[handle];
}

vertex& tmesh::get_v(vertex_handle handle)
{
    return vertices[handle];
}

const vertex& tmesh::get_v(vertex_handle handle) const
{
    return vertices[handle];
}

half_edge_handle tmesh::find_or_create_edge_between(new_face_vertex from_h, new_face_vertex to_h) {
    auto found_edge = get_half_edge_between(from_h.handle, to_h.handle);
    if (found_edge) {
        return found_edge.unwrap();
    } else {
        return add_edge_pair(from_h, to_h).first;
    }
}

pair<half_edge_handle, half_edge_handle> tmesh::add_edge_pair(new_face_vertex v1h, new_face_vertex v2h) {
    // This method adds two new half edges, called "a" and "b".
    //
    //  +----+  --------(a)-------->  +----+
    //  | v1 |                        | v2 |
    //  +----+  <-------(b)---------  +----+

    // Create incomplete/broken edges and edge handles. By the end of this
    // method, they are less invalid.
    half_edge a;
    half_edge b;

    // Add edges to our edge list.
    auto ah = edges.push(a);
    auto bh = edges.push(b);
    auto& a_inserted = get_e(ah);
    auto& b_inserted = get_e(bh);

    // Assign half-edge targets
    a_inserted.target = v2h.handle;
    b_inserted.target = v1h.handle;

    return make_pair(ah, bh);
}

edge_handle tmesh::half_to_full_edge_handle(half_edge_handle handle) const {
    auto twin = get_twin(handle);
    // return the handle with the smaller index of the given half edge and its twin
    return edge_handle(min(twin.get_idx(), handle.get_idx()));
}

}
