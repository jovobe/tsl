#include <algorithm>

#include <tsl/geometry/half_edge_mesh.hpp>

using std::min;
using std::tuple;
using std::get;

namespace tsl {

vertex_handle half_edge_mesh::add_vertex(vec3 pos)
{
    vertex v;
    v.pos = pos;
    return vertices.push(v);
}

face_handle half_edge_mesh::add_face(const vector<vertex_handle>& handles)
{
    if (!is_face_insertion_valid(handles))
    {
        panic("attempting to add a face which cannot be added!");
    }

    using std::make_tuple;

    // =======================================================================
    // = Create broken edges
    // =======================================================================
    // Handles for the inner edges of the face. The edges represented by those
    // handles do not contain a valid `next` and `prev` pointer yet.
    vector<half_edge_handle> inner_handles;
    inner_handles.reserve(handles.size());
    for (size_t i = 1; i < handles.size(); ++i) {
        auto inner_h = find_or_create_edge_between(handles[i - 1], handles[i]);
        inner_handles.push_back(inner_h);
    }
    auto inner_h = find_or_create_edge_between(handles.back(), handles.front());
    inner_handles.push_back(inner_h);

    // =======================================================================
    // = Create face
    // =======================================================================
    face_handle new_face_h = faces.next_handle();
    face f(inner_handles.front());
    faces.push(f);

    // Set face handle of edges and get handles for outer edges
    vector<half_edge_handle> outer_handles;
    outer_handles.reserve(handles.size());
    for (auto&& e_inner_h: inner_handles) {
        auto& e_inner = get_e(e_inner_h);
        e_inner.face = optional_face_handle(new_face_h);
        outer_handles.push_back(e_inner.twin);
    }

    // =======================================================================
    // = Fix next and prev handles and set outgoing handles if not set yet
    // =======================================================================
    // Fixing the `next` and `prev` handles is the most difficult part of this method. In
    // order to tackle it we deal with each corner of this face on its own.
    // For each corner we look at the corner-vertex and the in-going and
    // out-going edge (both edges are on the outside of this face!).
    vector<tuple<half_edge_handle, vertex_handle, half_edge_handle>> corners;
    corners.reserve(handles.size());
    corners.emplace_back(outer_handles.front(), handles.front(), outer_handles.back());
    for (size_t i = 1; i < handles.size(); ++i) {
        corners.emplace_back(outer_handles[i], handles[i], outer_handles[i - 1]);
    }

    // TODO: check for more special cases due to quad faces
    for (auto&& corner: corners)
    {
        auto e_in_h = get<0>(corner);
        auto vh = get<1>(corner);
        auto e_out_h = get<2>(corner);

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
                }).expect("a non-manifold part in the mesh has been found");

                auto e_start_h = get_e(e_end_h).next;
                auto& e_start = get_e(e_start_h);

                e_in.next = e_start_h;
                e_start.prev = e_in_h;

                get_e(e_end_h).next = e_out_h;
                e_out.prev = e_end_h;
            }
            else
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
            auto eh = get_e(e_in.twin).prev;

            get_e(eh).next = e_out_h;
            e_out.prev = eh;
        }
            // --> Case (C): only the outgoing edge is part of a face
        else if (!e_in.face && e_out.face)
        {
            // This is correct, because the old next pointer are still present!
            auto old_out_h = get_e(e_out.twin).next;
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
            if (get_e(e_out.twin).next != e_in.twin)
            {
                // Here we need to conceptually delete one fan blade from the
                // `next` and `prev` circle around `v` and re-insert it into the right
                // position. We choose to "move" the fan blade starting with
                // `e_in`.
                //
                // We search the edge that points to
                // `e_in.twin`.
                auto inactive_blade_end_h = get_e(e_in.twin).prev;

                // Instead of pointing to `e_in.twin`, it needs to "skip" the
                // `e_in` blade and point to the blade afterwards. So we need to
                // find the end of the `e_in` blade. Its next handle is the one
                // `inactive_blade_end` needs to point to.
                auto e_in_blade_end_h = find_edge_around_vertex(
                    e_in_h,
                    [&, this](auto edge_h)
                    {
                        return !get_e(edge_h).face;
                    }
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
                get_e(e_in_blade_end_h).next = get_e(e_out.twin).next;
                get_e(get_e(e_out.twin).next).prev = e_in_blade_end_h;
            }
        }
    }

    // Set `next` and `prev` handle of inner edges. This is an easy step, but we can't
    // do it earlier, since the old `next` and `prev` handles are required by the
    // previous "fix next and prev handles" step.
    auto& e_inner_first = get_e(inner_handles.front());
    e_inner_first.next = inner_handles[1];
    e_inner_first.prev = inner_handles.back();
    for (size_t i = 1; i < (inner_handles.size() - 1); ++i) {
        auto& e_inner = get_e(inner_handles[i]);
        e_inner.next = inner_handles[i + 1];
        e_inner.prev = inner_handles[i - 1];
    }
    auto& e_inner_last = get_e(inner_handles.back());
    e_inner_last.next = inner_handles.front();
    e_inner_last.prev = inner_handles[inner_handles.size() - 2];

    // Set outgoing-handles if they are not set yet.
    for (size_t i = 0; i < handles.size(); ++i) {
        auto& v = get_v(handles[i]);
        auto e_inner_h = inner_handles[i];
        if (!v.outgoing) {
            v.outgoing = optional_half_edge_handle(e_inner_h);
        }
    }

    return new_face_h;
}

size_t half_edge_mesh::num_vertices() const
{
    return vertices.num_used();
}

size_t half_edge_mesh::num_faces() const
{
    return faces.num_used();
}

size_t half_edge_mesh::num_edges() const
{
    return edges.num_used() / 2;
}

size_t half_edge_mesh::num_half_edges() const
{
    return edges.num_used();
}

vec3 half_edge_mesh::get_vertex_position(vertex_handle handle) const
{
    return get_v(handle).pos;
}

vec3& half_edge_mesh::get_vertex_position(vertex_handle handle)
{
    return get_v(handle).pos;
}

vector<vertex_handle> half_edge_mesh::get_vertices_of_face(face_handle handle) const
{
    vector<vertex_handle> vertices_out;
    vertices_out.reserve(4);
    circulate_in_face(handle, [&vertices_out, this](auto eh)
    {
        vertices_out.push_back(get_e(eh).target);
        return true;
    });
    return vertices_out;
}

array<vertex_handle, 2> half_edge_mesh::get_vertices_of_edge(edge_handle edge_h) const
{
    auto one_edge_h = half_edge_handle::one_half_of(edge_h);
    auto one_edge = get_e(one_edge_h);
    return { one_edge.target, get_e(one_edge.twin).target };
}

array<vertex_handle, 2> half_edge_mesh::get_vertices_of_half_edge(half_edge_handle edge_h) const {
    auto one_edge = get_e(edge_h);
    return { one_edge.target, get_e(one_edge.twin).target };
}

array<optional_face_handle, 2> half_edge_mesh::get_faces_of_edge(edge_handle edge_h) const
{
    auto one_edge_h = half_edge_handle::one_half_of(edge_h);
    auto one_edge = get_e(one_edge_h);
    return { one_edge.face, get_e(one_edge.twin).face };
}

void half_edge_mesh::get_edges_of_vertex(
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

void half_edge_mesh::get_half_edges_of_vertex(
    vertex_handle handle,
    vector<half_edge_handle>& edges_out
) const
{
    circulate_around_vertex(handle, [&edges_out, this](auto eh)
    {
        edges_out.push_back(eh);
        return true;
    });
}

vector<edge_handle> half_edge_mesh::get_edges_of_vertex(vertex_handle handle) const
{
    vector<edge_handle> out;
    get_edges_of_vertex(handle, out);
    return out;
}

vector<half_edge_handle> half_edge_mesh::get_half_edges_of_vertex(vertex_handle handle) const
{
    vector<half_edge_handle> out;
    get_half_edges_of_vertex(handle, out);
    return out;
}

vector<half_edge_handle> half_edge_mesh::get_half_edges_of_face(face_handle face_handle) const {
    vector<half_edge_handle> edges_out;
    edges_out.reserve(4);
    circulate_in_face(face_handle, [&edges_out, this](auto eh)
    {
        edges_out.push_back(eh);
        return true;
    });
    return edges_out;
}

optional_edge_handle half_edge_mesh::get_edge_between(vertex_handle ah, vertex_handle bh) const
{
    // Go through all edges of vertex `a` until we find an edge that is also
    // connected to vertex `b`.
    for (auto eh: get_edges_of_vertex(ah))
    {
        auto endpoints = get_vertices_of_edge(eh);
        if (endpoints[0] == bh || endpoints[1] == bh)
        {
            return optional_edge_handle(eh);
        }
    }
    return optional_edge_handle();
}

optional_half_edge_handle half_edge_mesh::get_half_edge_between(vertex_handle ah, vertex_handle bh) const
{
    // Go through all edges of vertex `a` until we find an edge that is also
    // connected to vertex `b`.
    for (auto eh: get_half_edges_of_vertex(ah))
    {
        auto edge = get_e(eh);
        if (get_e(edge.twin).target == bh)
        {
            return optional_half_edge_handle(edge.twin);
        }
    }
    return optional_half_edge_handle();
}

half_edge_handle half_edge_mesh::get_twin(half_edge_handle handle) const {
    return get_e(handle).twin;
}

half_edge_handle half_edge_mesh::get_prev(half_edge_handle handle) const {
    return get_e(handle).prev;
}

// TODO: this does not detect non-manifold edges!
bool half_edge_mesh::is_face_insertion_valid(const vector<vertex_handle>& handles) const
{
    // We are working with a quad mesh, so a face needs at least 4 vertices!
    if (handles.size() < 4) {
        return false;
    }

    // If there is already a face, we can't add another one
    if (get_face_between(handles))
    {
        return false;
    }

    // Next we check that each vertex is a boundary one (it has at least one
    // boundary edge or no edges at all). We really need this property;
    // otherwise we would create non-manifold vertices and make the mesh non-
    // orientable.
    vector<edge_handle> edges;
    for (auto&& vh: handles)
    {
        edges.clear();
        get_edges_of_vertex(vh, edges);

        // No edges at all are fine too.
        if (edges.empty())
        {
            continue;
        }

        // But if there are some, we need at least one boundary one.
        auto boundary_edge_it = std::find_if(edges.begin(), edges.end(), [&, this](auto eh)
        {
            return this->num_adjacent_faces(eh) < 2;
        });
        if (boundary_edge_it == edges.end())
        {
            return false;
        }
    }

    return true;
}

// TODO: This is more compilcated! what if a face of v0, v1, v2, v3 exists and we try to create a face
//       with v0-v6? Search for more edge cases!
optional_face_handle half_edge_mesh::get_face_between(const vector<vertex_handle>& handles) const
{
    if (handles.size() < 4) {
        panic("too few vertex handles! A face in a quad mesh needs at least four vertices!");
    }

    // Start with one edge
    const auto ab_edge_h = get_edge_between(handles[0], handles[1]);

    // If the edge already doesn't exist, there won't be a face either.
    if (!ab_edge_h)
    {
        return optional_face_handle();
    }

    // The two faces of the edge. One of these faces should contain the vertex
    // `c` or there is just no face between the given vertices.
    const auto faces = get_faces_of_edge(ab_edge_h.unwrap());

    // Find the face which contains vertex `c`.
    auto face_it = std::find_if(faces.begin(), faces.end(), [&, this](auto maybe_face_h)
    {
        if (!maybe_face_h)
        {
            return false;
        }
        const auto vertices = this->get_vertices_of_face(maybe_face_h.unwrap());
        return std::find(vertices.begin(), vertices.end(), handles[2]) != vertices.end();
    });

    return face_it != faces.end() ? *face_it : optional_face_handle();
}

uint8_t half_edge_mesh::num_adjacent_faces(edge_handle handle) const
{
    auto faces = get_faces_of_edge(handle);
    return static_cast<uint8_t>((faces[0] ? 1 : 0) + (faces[1] ? 1 : 0));
}

typename half_edge_mesh::edge&
half_edge_mesh::get_e(half_edge_handle handle)
{
    return edges[handle];
}

const typename half_edge_mesh::edge&
half_edge_mesh::get_e(half_edge_handle handle) const
{
    return edges[handle];
}

typename half_edge_mesh::face&
half_edge_mesh::get_f(face_handle handle)
{
    return faces[handle];
}

const typename half_edge_mesh::face&
half_edge_mesh::get_f(face_handle handle) const
{
    return faces[handle];
}

typename half_edge_mesh::vertex&
half_edge_mesh::get_v(vertex_handle handle)
{
    return vertices[handle];
}

const typename half_edge_mesh::vertex&
half_edge_mesh::get_v(vertex_handle handle) const
{
    return vertices[handle];
}

edge_handle half_edge_mesh::half_to_full_edge_handle(half_edge_handle handle) const
{
    auto twin = get_e(handle).twin;
    // return the handle with the smaller index of the given half edge and its twin
    return edge_handle(min(twin.get_idx(), handle.get_idx()));
}

optional_half_edge_handle half_edge_mesh::edge_between(vertex_handle from_h, vertex_handle to_h)
{
    auto ingoing_edge = find_edge_around_vertex(from_h, [&, this](auto current_edge_h)
    {
        return get_e(get_e(current_edge_h).twin).target == to_h;
    });
    if (ingoing_edge)
    {
        return optional_half_edge_handle(get_e(ingoing_edge.unwrap()).twin);
    }
    else
    {
        return optional_half_edge_handle();
    }
}

half_edge_handle half_edge_mesh::find_or_create_edge_between(vertex_handle from_h, vertex_handle to_h)
{
    auto found_edge = edge_between(from_h, to_h);
    if (found_edge)
    {
        return found_edge.unwrap();
    }
    else
    {
        return add_edge_pair(from_h, to_h).first;
    }
}

pair<half_edge_handle, half_edge_handle> half_edge_mesh::add_edge_pair(vertex_handle v1h, vertex_handle v2h)
{
    // This method adds two new half edges, called "a" and "b".
    //
    //  +----+  --------(a)-------->  +----+
    //  | v1 |                        | v2 |
    //  +----+  <-------(b)---------  +----+

    // Create incomplete/broken edges and edge handles. By the end of this
    // method, they are less invalid.
    edge a;
    edge b;

    // Add edges to our edge list.
    auto ah = edges.push(a);
    auto bh = edges.push(b);
    auto& a_inserted = get_e(ah);
    auto& b_inserted = get_e(bh);

    // Assign twins to each other
    a_inserted.twin = bh;
    b_inserted.twin = ah;

    // Assign half-edge targets
    a_inserted.target = v2h;
    b_inserted.target = v1h;

    return std::make_pair(ah, bh);
}

hem_iterator<vertex_handle, half_edge_vertex> half_edge_mesh::vertices_begin() const
{
    return hem_iterator<vertex_handle, half_edge_vertex>(vertices.begin());
}

hem_iterator<vertex_handle, half_edge_vertex> half_edge_mesh::vertices_end() const
{
    return hem_iterator<vertex_handle, half_edge_vertex>(vertices.end());
}

hem_iterator<face_handle, half_edge_face> half_edge_mesh::faces_begin() const
{
    return hem_iterator<face_handle, half_edge_face>(faces.begin());
}

hem_iterator<face_handle, half_edge_face> half_edge_mesh::faces_end() const
{
    return hem_iterator<face_handle, half_edge_face>(faces.end());
}

hem_iterator<half_edge_handle, half_edge> half_edge_mesh::half_edges_begin() const
{
    return hem_iterator<half_edge_handle, half_edge>(edges.begin());
}

hem_iterator<half_edge_handle, half_edge> half_edge_mesh::half_edges_end() const
{
    return hem_iterator<half_edge_handle, half_edge>(edges.end());
}

hem_face_iterator_proxy half_edge_mesh::get_faces() const
{
    return hem_face_iterator_proxy(*this);
}

hem_half_edge_iterator_proxy half_edge_mesh::get_half_edges() const
{
    return hem_half_edge_iterator_proxy(*this);
}

hem_vertex_iterator_proxy half_edge_mesh::get_vertices() const
{
    return hem_vertex_iterator_proxy(*this);
}

hem_iterator<face_handle, half_edge_face> hem_face_iterator_proxy::begin() const
{
    return mesh.faces_begin();
}

hem_iterator<face_handle, half_edge_face> hem_face_iterator_proxy::end() const
{
    return mesh.faces_end();
}

hem_iterator<half_edge_handle, half_edge> hem_half_edge_iterator_proxy::begin() const
{
    return mesh.half_edges_begin();
}

hem_iterator<half_edge_handle, half_edge> hem_half_edge_iterator_proxy::end() const
{
    return mesh.half_edges_end();
}

hem_iterator<vertex_handle, half_edge_vertex> hem_vertex_iterator_proxy::begin() const
{
    return mesh.vertices_begin();
}

hem_iterator<vertex_handle, half_edge_vertex> hem_vertex_iterator_proxy::end() const
{
    return mesh.vertices_end();
}

}
