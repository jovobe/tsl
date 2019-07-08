#include "tsl/geometry/tmesh/iterator.hpp"

namespace tsl {

tmesh_edge_iterator& tmesh_edge_iterator::operator++()
{
    ++iterator;

    // If not at the end, find the next half edge handle that equals the full edge handle of that edge
    // according to the halfToFullEdgeHandle method
    while (!iterator.is_at_end() && (*iterator).get_idx() != mesh.half_to_full_edge_handle(*iterator).get_idx())
    {
        ++iterator;
    }

    return *this;
}

bool tmesh_edge_iterator::operator==(const tmesh_iterator<edge_handle>& other) const
{
    auto cast = dynamic_cast<const tmesh_edge_iterator*>(&other);
    return cast && iterator == cast->iterator;
}

bool tmesh_edge_iterator::operator!=(const tmesh_iterator<edge_handle>& other) const
{
    auto cast = dynamic_cast<const tmesh_edge_iterator*>(&other);
    return !cast || iterator != cast->iterator;
}

edge_handle tmesh_edge_iterator::operator*() const
{
    return mesh.half_to_full_edge_handle(*iterator);
}

tmesh_iterator_ptr<face_handle> tmesh_face_iterator_proxy::begin() const
{
    return mesh.faces_begin();
}

tmesh_iterator_ptr<face_handle> tmesh_face_iterator_proxy::end() const
{
    return mesh.faces_end();
}

tmesh_iterator_ptr<half_edge_handle> tmesh_half_edge_iterator_proxy::begin() const
{
    return mesh.half_edges_begin();
}

tmesh_iterator_ptr<half_edge_handle> tmesh_half_edge_iterator_proxy::end() const
{
    return mesh.half_edges_end();
}

tmesh_iterator_ptr<edge_handle> tmesh_edge_iterator_proxy::begin() const
{
    return mesh.edges_begin();
}

tmesh_iterator_ptr<edge_handle> tmesh_edge_iterator_proxy::end() const
{
    return mesh.edges_end();
}

tmesh_iterator_ptr<vertex_handle> tmesh_vertex_iterator_proxy::begin() const
{
    return mesh.vertices_begin();
}

tmesh_iterator_ptr<vertex_handle> tmesh_vertex_iterator_proxy::end() const
{
    return mesh.vertices_end();
}

}
