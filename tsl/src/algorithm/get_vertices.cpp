#include "tsl/algorithm/get_vertices.hpp"

namespace tsl {

/*
vector<vector<vertex_handle>> get_regular_rings_around_vertex(const tmesh& mesh, vertex_handle handle, uint32_t rings) {
    assert(rings > 0);
    auto outgoing = mesh.get_half_edges_of_vertex(handle, edge_direction::outgoing);

    // create space for vertices ordered by ring
    vector<vector<vertex_handle>> vertices;
    {
        auto valence = mesh.get_valence(handle);
        vertices.reserve(rings);
        for (uint32_t i = 0; i < rings; ++i) {
            vertices.emplace_back();

            // every ring holds: ring * valence * 2 vertices
            vertices.back().reserve((i + 1) * valence * 2);
        }
    }

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
                vertices[ring].push_back(mesh.get_target(loop_edge));
                loop_edge = mesh.get_next(loop_edge);

                // march line in ring
                for (uint32_t j = 0; j < ring; ++j) {
                    vertices[ring].push_back(mesh.get_target(loop_edge));
                    loop_edge = mesh.get_next(mesh.get_twin(mesh.get_next(loop_edge)));
                }
            }
        }
    }

    return vertices;
}
 */

vector<vertex_handle> get_regular_rings_around_vertex(const tmesh& mesh, vertex_handle handle, uint32_t rings) {
    // create space for vertices ordered by ring
    vector<vertex_handle> vertices;
    {
        // every ring holds: ring * valence * 2 vertices, because we will add all rings into one vector, we need
        // the "sum of rings" this is the factorial but with addition, which is computed by the triangle number
        // which can be calculated by the formula: (n^2 + n) / 2
        auto valence = mesh.get_valence(handle);
        auto sum_rings = ((rings * rings) + rings) / 2;
        vertices.reserve(sum_rings * valence * 2);
    }

    visit_regular_rings(mesh, handle, rings, [&vertices](auto handle, auto ring) {
        vertices.push_back(handle);
        return true;
    });

    return vertices;
}

vector<vertex_handle> get_extraordinary_vertices_in_regular_rings_around_vertex(const tmesh& mesh, vertex_handle handle, uint32_t rings) {
    // create space for vertices ordered by ring
    vector<vertex_handle> vertices;

    visit_regular_rings(mesh, handle, rings, [&vertices, &mesh](auto handle, auto ring) {
        if (mesh.is_extraordinary(handle)) {
            vertices.push_back(handle);
        }
        return true;
    });

    return vertices;
}

}
