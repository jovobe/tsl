namespace tsl {

template<typename visitor_t>
void tmesh::circulate_around_vertex(vertex_handle vh, visitor_t visitor, edge_direction way) const {
    auto outgoing = get_v(vh).outgoing;
    if (outgoing) {
        switch (way) {
            case edge_direction::ingoing:
                circulate_around_vertex(get_twin(outgoing.unwrap()), visitor, way);
                break;
            case edge_direction::outgoing:
                circulate_around_vertex(outgoing.unwrap(), visitor, way);
                break;
        }
    }
}

template<typename visitor_t>
void tmesh::circulate_around_vertex(half_edge_handle start_edge_h, visitor_t visitor, edge_direction way) const {
    auto loop_edge_h = start_edge_h;

    while (true) {
        // Call the visitor and stop, if the visitor tells us to.
        if (!visitor(loop_edge_h)) {
            break;
        }

        // Advance to next edge and stop if it is the start edge.
        switch (way) {
            case edge_direction::ingoing:
                loop_edge_h = get_twin(get_e(loop_edge_h).next);
                break;
            case edge_direction::outgoing:
                loop_edge_h = get_e(get_twin(loop_edge_h)).next;
                break;
            default:
                panic("unhandled edge_direction in circulate_around_vertex!");
        }

        if (loop_edge_h == start_edge_h) {
            break;
        }
    }
}

template<typename visitor_t>
void tmesh::circulate_in_face(face_handle fh, visitor_t visitor) const {
    circulate_in_face(get_f(fh).edge, visitor);
}

template<typename visitor_t>
void tmesh::circulate_in_face(half_edge_handle start_edge_h, visitor_t visitor) const {
    auto loop_edge_h = start_edge_h;

    while (true) {
        // Call the visitor and stop, if the visitor tells us to.
        if (!visitor(loop_edge_h)) {
            break;
        }

        // Advance to next edge and stop if it is the start edge.
        loop_edge_h = get_e(loop_edge_h).next;
        if (loop_edge_h == start_edge_h) {
            break;
        }
    }
}

template<typename pred_t>
optional_half_edge_handle tmesh::find_edge_around_vertex(vertex_handle vh, pred_t pred, edge_direction way) const {
    // This function simply follows `next` and `twin` handles to visit all
    // edges around a vertex.
    optional_half_edge_handle out;
    auto& v = get_v(vh);
    if (!v.outgoing) {
        return out;
    }

    switch (way) {
        case edge_direction::ingoing:
            out = find_edge_around_vertex(get_twin(v.outgoing.unwrap()), pred, way);
            break;
        case edge_direction::outgoing:
            out = find_edge_around_vertex(v.outgoing.unwrap(), pred, way);
            break;
    }

    return out;
}

template<typename pred_t>
optional_half_edge_handle tmesh::find_edge_around_vertex(half_edge_handle start_edge_h, pred_t pred, edge_direction way) const {
    // This function simply follows `next` and `twin` handles to visit all
    // edges around a vertex.
    optional_half_edge_handle out;
    circulate_around_vertex(start_edge_h, [&](auto ingoing_edge_h) {
        if (pred(ingoing_edge_h)) {
            out = optional_half_edge_handle(ingoing_edge_h);
            return false;
        }
        return true;
    }, way);

    return out;
}

}
