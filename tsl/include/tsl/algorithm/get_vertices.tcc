namespace tsl {

template <typename visitor_t>
void visit_regular_rings(const tmesh& mesh, vertex_handle handle, uint32_t rings, visitor_t visitor) {
    assert(rings > 0);
    auto outgoing = mesh.get_half_edges_of_vertex(handle, edge_direction::outgoing);

    // circulate around vertex and handle every "quadrant" in one loop
    for (auto&& heh: outgoing) {

        // for each ring
        for (uint32_t ring = 0; ring < rings; ++ring) {
            auto loop_edge = heh;

            // march forward to ring
            for (uint32_t j = 0; j < ring; ++j) {
                loop_edge = mesh.get_next(mesh.get_twin(mesh.get_next(loop_edge)));
            }

            // thanks to symmetry we need to do this exactly twice
            for (uint32_t k = 0; k < 2; ++k) {
                if (!visitor(mesh.get_target(loop_edge), ring)) {
                    return;
                }
                loop_edge = mesh.get_next(loop_edge);

                // march line in ring
                for (uint32_t j = 0; j < ring; ++j) {
                    if (!visitor(mesh.get_target(loop_edge), ring)) {
                        return;
                    }
                    loop_edge = mesh.get_next(mesh.get_twin(mesh.get_next(loop_edge)));
                }
            }
        }
    }
}

}
