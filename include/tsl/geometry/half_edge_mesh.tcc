namespace tsl {

template <typename visitor_t>
void half_edge_mesh::circulate_in_face(face_handle fh, visitor_t visitor) const
{
    circulate_in_face(get_f(fh).edge, visitor);
}

template <typename visitor_t>
void half_edge_mesh::circulate_in_face(half_edge_handle start_edge_h, visitor_t visitor) const
{
    auto loop_edge_h = start_edge_h;

    while (true)
    {
        // Call the visitor and stop, if the visitor tells us to.
        if (!visitor(loop_edge_h))
        {
            break;
        }

        // Advance to next edge and stop if it is the start edge.
        loop_edge_h = get_e(loop_edge_h).next;
        if (loop_edge_h == start_edge_h)
        {
            break;
        }
    }
}

template <typename visitor_t>
void half_edge_mesh::circulate_around_vertex(vertex_handle vh, visitor_t visitor) const
{
    auto outgoing = get_v(vh).outgoing;
    if (outgoing)
    {
        circulate_around_vertex(get_e(outgoing.unwrap()).twin, visitor);
    }
}

template <typename visitor_t>
void half_edge_mesh::circulate_around_vertex(half_edge_handle start_edge_h, visitor_t visitor) const
{
    auto loop_edge_h = start_edge_h;

    while (true)
    {
        // Call the visitor and stop, if the visitor tells us to.
        if (!visitor(loop_edge_h))
        {
            break;
        }

        // Advance to next edge and stop if it is the start edge.
        loop_edge_h = get_e(get_e(loop_edge_h).next).twin;
        if (loop_edge_h == start_edge_h)
        {
            break;
        }
    }
}

template <typename pred_t>
optional_half_edge_handle half_edge_mesh::find_edge_around_vertex(vertex_handle vh, pred_t pred) const
{
    // This function simply follows `next` and `twin` handles to visit all
    // edges around a vertex.
    auto& v = get_v(vh);
    if (!v.outgoing)
    {
        return optional_half_edge_handle();
    }

    return find_edge_around_vertex(get_e(v.outgoing.unwrap()).twin, pred);
}

template <typename pred_t>
optional_half_edge_handle half_edge_mesh::find_edge_around_vertex(half_edge_handle start_edge_h, pred_t pred) const
{
    // This function simply follows `next` and `twin` handles to visit all
    // edges around a vertex.
    optional_half_edge_handle out;
    circulate_around_vertex(start_edge_h, [&, this](auto ingoing_edge_h)
    {
        if (pred(ingoing_edge_h))
        {
            out = optional_half_edge_handle(ingoing_edge_h);
            return false;
        }
        return true;
    });

    return out;
}

template<typename handle_t, typename elem_t>
hem_iterator<handle_t, elem_t>& hem_iterator<handle_t, elem_t>::operator++()
{
    ++iterator;
    return *this;
}

template<typename handle_t, typename elem_t>
bool hem_iterator<handle_t, elem_t>::operator==(const hem_iterator<handle_t, elem_t>& other) const
{
    return iterator == other.iterator;
}

template<typename handle_t, typename elem_t>
bool hem_iterator<handle_t, elem_t>::operator!=(const hem_iterator<handle_t, elem_t>& other) const
{
    return iterator != other.iterator;
}

template<typename handle_t, typename elem_t>
handle_t hem_iterator<handle_t, elem_t>::operator*() const
{
    return *iterator;
}

}