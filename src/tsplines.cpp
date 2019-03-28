#include <queue>
#include <algorithm>
#include <variant>

#include <tsl/tsplines.hpp>
#include <tsl/geometry/line.hpp>
#include <tsl/geometry/rectangle.hpp>

using std::queue;
using std::min;
using std::max;
using std::get;

namespace tsl {

tmesh tsplines::get_example_data_1() {
    tmesh out;
    auto& mesh = out.mesh;

    const float EDGE_LENGTH = 1.0f;

    // =============================================
    // FRONT SIDE
    // =============================================
    //
    //       (v4) ====== (v1) ====== (v0)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f1    ||   f0     ||
    //        ||          ||          ||
    //       (v5) ====== (v2) ====== (v3)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f3    ||   f2     ||
    //   z    ||          ||          ||
    //   ↑   (v8) ====== (v7) ====== (v6)
    //   |
    // (0,0) ---> x

    // f0
    auto v0 = mesh.add_vertex({2.0f, 0.0f, 2.0f});
    auto v1 = mesh.add_vertex({1.0f, 0.0f, 2.0f});
    auto v2 = mesh.add_vertex({1.0f, 0.0f, 1.0f});
    auto v3 = mesh.add_vertex({2.0f, 0.0f, 1.0f});
    mesh.add_face({v0, v1, v2, v3});

    // f1
    auto v4 = mesh.add_vertex({0.0f, 0.0f, 2.0f});
    auto v5 = mesh.add_vertex({0.0f, 0.0f, 1.0f});
    mesh.add_face({v1, v4, v5, v2});

    // f2
    auto v6 = mesh.add_vertex({2.0f, 0.0f, 0.0f});
    auto v7 = mesh.add_vertex({1.0f, 0.0f, 0.0f});
    mesh.add_face({v3, v2, v7, v6});

    // f3
    auto v8 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    mesh.add_face({v2, v5, v8, v7});

    // =============================================
    // LEFT SIDE
    // =============================================
    //
    //       (v11) ===== (v9) ====== (v4)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f5    ||   f4     ||
    //        ||          ||          ||
    //       (v12) ===== (v10) ===== (v5)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f7    ||   f6     ||
    //   z    ||          ||          ||
    //   ↑   (v14) ===== (v13) ===== (v8)
    //   |
    // (-2,0) ---> y

    // f4
    auto v9 = mesh.add_vertex({0.0f, 1.0f, 2.0f});
    auto v10 = mesh.add_vertex({0.0f, 1.0f, 1.0f});
    mesh.add_face({v4, v9, v10, v5});

    // f5
    auto v11 = mesh.add_vertex({0.0f, 2.0f, 2.0f});
    auto v12 = mesh.add_vertex({0.0f, 2.0f, 1.0f});
    mesh.add_face({v9, v11, v12, v10});

    // f6
    auto v13 = mesh.add_vertex({0.0f, 1.0f, 0.0f});
    mesh.add_face({v5, v10, v13, v8});

    // f7
    auto v14 = mesh.add_vertex({0.0f, 2.0f, 0.0f});
    mesh.add_face({v10, v12, v14, v13});

    // =============================================
    // BACK SIDE
    // =============================================
    //
    //       (v17) ===== (v15) ===== (v11)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f9    ||   f8     ||
    //        ||          ||          ||
    //       (v18) ===== (v16) ===== (v12)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f11   ||   f10    ||
    //        ||          ||          ||   z
    //       (v20) ===== (v19) ===== (v14) ↑
    //                                     |
    //                               x <--- (0,0)

    // f8
    auto v15 = mesh.add_vertex({1.0f, 2.0f, 2.0f});
    auto v16 = mesh.add_vertex({1.0f, 2.0f, 1.0f});
    mesh.add_face({v11, v15, v16, v12});

    // f9
    auto v17 = mesh.add_vertex({2.0f, 2.0f, 2.0f});
    auto v18 = mesh.add_vertex({2.0f, 2.0f, 1.0f});
    mesh.add_face({v15, v17, v18, v16});

    // f10
    auto v19 = mesh.add_vertex({1.0f, 2.0f, 0.0f});
    mesh.add_face({v12, v16, v19, v14});

    // f11
    auto v20 = mesh.add_vertex({2.0f, 2.0f, 0.0f});
    mesh.add_face({v16, v18, v20, v19});

    // =============================================
    // RIGHT SIDE
    // =============================================
    //
    //       (v0) ====== (v21) ===== (v17)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f13   ||   f12    ||
    //        ||          ||          ||
    //       (v3) ====== (v22) ===== (v18)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f15   ||   f14    ||
    //   z    ||          ||          ||
    //   ↑   (v6) ====== (v23) ===== (v20)
    //   |
    // (0,0) ---> y

    // f12
    auto v21 = mesh.add_vertex({2.0f, 1.0f, 2.0f});
    auto v22 = mesh.add_vertex({2.0f, 1.0f, 1.0f});
    mesh.add_face({v17, v21, v22, v18});

    // f13
    mesh.add_face({v21, v0, v3, v22});

    // f14
    auto v23 = mesh.add_vertex({2.0f, 1.0f, 0.0f});
    mesh.add_face({v18, v22, v23, v20});

    // f15
    mesh.add_face({v22, v3, v6, v23});

    // =============================================
    // TOP SIDE
    // =============================================
    //
    //                   BACK
    //
    //       (v11) ===== (v15) ===== (v17)
    //        ||          ||          ||
    //        ||          ||          ||
    //     L  ||    f17   ||   f16    ||   R
    //     E  ||          ||          ||   I
    //     F (v9) ====== (v24) ===== (v21) G
    //     T  ||          ||          ||   H
    //        ||          ||          ||   T
    //        ||    f19   ||   f18    ||
    //        ||          ||          ||
    //       (v4) ====== (v1) ====== (v0)
    //
    //                   FRONT

    // f16
    auto v24 = mesh.add_vertex({1.0f, 1.0f, 2.0f});
    mesh.add_face({v17, v15, v24, v21});

    // f17
    mesh.add_face({v15, v11, v9, v24});

    // f18
    mesh.add_face({v21, v24, v1, v0});

    // f19
    mesh.add_face({v24, v9, v4, v1});

    // =============================================
    // BOTTOM SIDE
    // =============================================
    //
    //                   FRONT
    //
    //       (v8) ====== (v7) ====== (v6)
    //        ||          ||          ||
    //        ||          ||          ||
    //     L  ||    f21   ||   f20    ||   R
    //     E  ||          ||          ||   I
    //     F (v13) ===== (v25) ===== (v23) G
    //     T  ||          ||          ||   H
    //        ||          ||          ||   T
    //        ||    f23   ||   f22    ||
    //        ||          ||          ||
    //       (v14) ===== (v19) ===== (v20)
    //
    //                   BACK

    // f20
    auto v25 = mesh.add_vertex({1.0f, 1.0f, 0.0f});
    mesh.add_face({v6, v7, v25, v23});

    // f21
    mesh.add_face({v7, v8, v13, v25});

    // f22
    mesh.add_face({v23, v25, v19, v20});

    // f23
    mesh.add_face({v25, v13, v14, v19});

    // Scale mesh
    for (auto&& vh: mesh.get_vertices()) {
        auto& vpos = mesh.get_vertex_position(vh);
        vpos *= EDGE_LENGTH;
    }

    // Set corners and knots
    out.knots = dense_half_edge_map<float>(1.0f);
    out.corners = dense_half_edge_map<bool>(true);

    // Calc basis funs
    auto [uv, dir] = out.determine_local_coordinate_systems();
    auto transitions = out.determine_edge_transitions(uv, dir);
    auto [handles, transforms] = out.setup_basis_function_handles_and_transitions(uv, dir);
    auto knot_vectors = out.determine_knot_vectors(handles);

    out.handles = handles;
    out.knot_vectors = knot_vectors;
    out.support_map = out.determine_support_of_basis_functions(handles, transforms, transitions, uv, knot_vectors);
    out.uv = uv;
    out.dir = dir;
    out.edge_transitions = transitions;

    return out;
}

tmesh tsplines::get_example_data_2(uint32_t size) {
    const double EDGE_LENGTH = 1.0;

    tmesh out;
    out.mesh = half_edge_mesh::as_cube(EDGE_LENGTH, size, true);

    // Set corners and knots
    out.knots = dense_half_edge_map<float>(1.0f);
    out.corners = dense_half_edge_map<bool>(true);

    auto edge1 = out.mesh.get_half_edge_between(vertex_handle((2 * size) + 2), vertex_handle((3 * size) + 2)).unwrap();
    auto edge2 = out.mesh.get_half_edge_between(vertex_handle((4 * size) + 3), vertex_handle((3 * size) + 3)).unwrap();
    auto edge3 = out.mesh.get_half_edge_between(vertex_handle((1 * size) + 3), vertex_handle((1 * size) + 2)).unwrap();
    auto edge4 = out.mesh.get_half_edge_between(vertex_handle((2 * size) + 1), vertex_handle((2 * size) + 2)).unwrap();
    out.corners[edge1] = false;
    out.corners[edge2] = false;
    out.corners[edge3] = false;
    out.corners[edge4] = false;

    // Calc basis funs
    auto [uv, dir] = out.determine_local_coordinate_systems();
    auto transitions = out.determine_edge_transitions(uv, dir);
    auto [handles, transforms] = out.setup_basis_function_handles_and_transitions(uv, dir);
    auto knot_vectors = out.determine_knot_vectors(handles);

    out.handles = handles;
    out.knot_vectors = knot_vectors;
    out.support_map = out.determine_support_of_basis_functions(handles, transforms, transitions, uv, knot_vectors);
    out.uv = uv;
    out.dir = dir;
    out.edge_transitions = transitions;

    return out;
}

tuple<dense_half_edge_map<vec2>, dense_half_edge_map<uint8_t>>
tmesh::determine_local_coordinate_systems() const {
    dense_half_edge_map<vec2> uv;
    dense_half_edge_map<uint8_t> dir;
    uv.reserve(mesh.num_half_edges());
    dir.reserve(mesh.num_half_edges());

    for (auto&& fh: mesh.get_faces()) {
        vec2 c(0.0f, 0.0f);
        uint8_t i = 0;

        auto edges = mesh.get_half_edges_of_face(fh);
        for (auto&& eh: edges) {
            auto k = knots[eh];
            c += rotate(i, vec2(k, 0));

            uv.insert(eh, c);
            dir.insert(eh, i);

            if (corners[eh]) {
                i += 1;
            }
        }
    }

    return {uv, dir};
}

dense_half_edge_map<transform>
tmesh::determine_edge_transitions(const dense_half_edge_map<vec2>& uv, const dense_half_edge_map<uint8_t>& dir) const {
    dense_half_edge_map<transform> out;
    out.reserve(mesh.num_half_edges());

    for (auto&& eh: mesh.get_half_edges()) {
        auto twin = mesh.get_twin(eh);
        auto f = fac(twin);
        auto r = static_cast<uint8_t>((dir[twin] - dir[eh] + 6) % 4);
        auto t = uv[mesh.get_prev(twin)] - (f * rotate(r, uv[eh]));
        out.insert(eh, transform(f, r, t));
    }

    return out;
}

tuple<unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>>, unordered_map<indexed_vertex_handle, transform>>
tmesh::setup_basis_function_handles_and_transitions(const dense_half_edge_map<vec2>& uv,
                                                    const dense_half_edge_map<uint8_t>& dir) const {
    unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>> handles;
    unordered_map<indexed_vertex_handle, transform> transforms;
    handles.reserve(mesh.num_vertices());
    transforms.reserve(mesh.num_vertices());

    for (auto&& vh: mesh.get_vertices()) {
        // TODO: Perhaps skip extraordinary vertices as well?

        uint32_t i = 0;
        for (auto&& eh: mesh.get_half_edges_of_vertex(vh, direction::outgoing)) {
            // TODO: Check, if this should be done or not (will be commented out until this is clarified)
            // Skip extraordinary vertices
//            if (is_extraordinary(mesh.get_target(mesh.get_next(eh)))) {
//                continue;
//            }

            handles.insert({indexed_vertex_handle(vh, i), {eh, tag::positive_u}});
            auto r = static_cast<uint8_t>(4 - dir[eh]);
            auto t = -rotate(r, uv[mesh.get_prev(eh)]);
            transforms.insert({indexed_vertex_handle(vh, i), transform(1, r, t)});
            i += 1;

            auto twin = mesh.get_twin(eh);
            if (!corners[twin]) {
                auto next_of_twin = mesh.get_next(twin);
                handles.insert({indexed_vertex_handle(vh, i), {next_of_twin, tag::negative_v}});
                r = static_cast<uint8_t>((4 - dir[next_of_twin] - 1) % 4);
                t = -rotate(r, uv[twin]);
                transforms.insert({indexed_vertex_handle(vh, i), transform(1, r, t)});
                i += 1;
            }
        }
    }

    return {handles, transforms};
}

unordered_map<double_indexed_vertex_handle, float>
tmesh::determine_knot_vectors(const unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>>& handles) const {
    unordered_map<double_indexed_vertex_handle, float> out;
    out.reserve(handles.size() * 2);

    for (auto&& [k, v] : handles) {
        auto [h, q] = v;
        float s = 0.0f;
        uint32_t j = 1;

        // no edge in u direction (T-joint), so walk “around” face
        if (q == tag::negative_v) {
            while (!from_corner(h)) {
                s += knots[h];
                h = mesh.get_next(h);
            }
        }

        bool skip = false;
        do {
            auto handle = double_indexed_vertex_handle(k.vertex, k.index, static_cast<uint8_t>(j));
            if (out.count(handle) == 0) {
                out[handle] = 0.0f;
            }
            out[handle] += knots[h];

            // first intersection on ray encountered
            if (s == 0) {
                j += 1;
            }

            if (j > 2) {
                skip = true;
                break;
            }

            h = mesh.get_next(h);
        } while(!from_corner(h));

        if (skip) {
            continue;
        }

        j = 2;

        while (s >= knots[h]) {
            s -= knots[h];
            h = mesh.get_next(h);
        }

        auto f = fac(h);
        h = mesh.get_twin(h);

        while (!corners[h]) {
            h = mesh.get_next(h);
            s += f * knots[h];
        }

        h = mesh.get_next(h);
        skip = false;
        do {
            auto handle = double_indexed_vertex_handle(k.vertex, k.index, static_cast<uint8_t>(j));
            if (out.count(handle) == 0) {
                out[handle] = 0.0f;
            }
            out[handle] += f * knots[h];

            if (s == 0) {
                skip = true;
                break;
            }

            h = mesh.get_next(h);
        } while(!from_corner(h));

        if (skip) {
            continue;
        }
    }

    return out;
}

dense_face_map<vector<tuple<indexed_vertex_handle, transform>>>
tmesh::determine_support_of_basis_functions(
    const unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>>& handles,
    const unordered_map<indexed_vertex_handle, transform>& transforms,
    const dense_half_edge_map<transform>& edge_transforms,
    const dense_half_edge_map<vec2>& uv,
    const unordered_map<double_indexed_vertex_handle, float>& knot_vectors
) const {
    dense_face_map<vector<tuple<indexed_vertex_handle, transform>>> out;
    out.reserve(mesh.num_faces());

    for (auto&& [k, v] : handles) {
        // TODO: Reset tagged map instead of recreating it every handle
        sparse_face_map<bool> tagged(false);
        // Reserve space for (in case of degree = 3 at least 8) faces
        tagged.reserve(DEGREE * DEGREE);

        auto [h, q] = v;
        queue<tuple<half_edge_handle, transform>> bfs_queue;

        bfs_queue.push({h, transforms.at(k)});
        auto face_h = mesh.get_face_of_half_edge(h).expect(EXPECT_NO_BORDER);
        tagged[face_h] = true;

        auto r = get_parametric_domain(k, knot_vectors, handles);

        while (!bfs_queue.empty()) {
            auto [ch, ct] = bfs_queue.front();
            bfs_queue.pop();
            auto cface_h = mesh.get_face_of_half_edge(ch).expect(EXPECT_NO_BORDER);

            if (!out.contains_key(cface_h)) {
                out.insert(cface_h, vector<tuple<indexed_vertex_handle, transform>>());
            }
//            out[cface_h].emplace_back(k, ct);

            // TODO: Why is the same k added twice here? This check should not be necessary...
            auto passed_k = k;
            auto find_key = std::find_if(out[cface_h].begin(), out[cface_h].end(), [&](auto tuple)
            {
                return get<0>(tuple).vertex == passed_k.vertex;
            });
            if (find_key == out[cface_h].end())
            {
                out[cface_h].emplace_back(k, ct);
            }

            auto g = mesh.get_next(ch);
            while (g != ch) {
                auto prev_g = mesh.get_prev(g);
                line l(ct.apply(uv[g]), ct.apply(uv[prev_g]));
                if (!l.intersects(r)) {
                    g = mesh.get_next(g);
                    continue;
                }

                auto twin = mesh.get_twin(g);
                auto twin_face_h = mesh.get_face_of_half_edge(twin).expect(EXPECT_NO_BORDER);
                if (tagged[twin_face_h]) {
                    g = mesh.get_next(g);
                    continue;
                }
                tagged[twin_face_h] = true;

                bfs_queue.push({twin, ct.apply(edge_transforms[twin])});

                g = mesh.get_next(g);
            }
        }
    }

    return out;
}

float tmesh::fac(half_edge_handle handle) const {
    return knots[handle] / knots[mesh.get_twin(handle)];
}

bool tmesh::from_corner(half_edge_handle handle) const {
    return corners[mesh.get_prev(handle)];
}

aa_rectangle tmesh::get_parametric_domain(
    const indexed_vertex_handle& handle,
    const unordered_map<double_indexed_vertex_handle, float>& knot_vectors,
    const unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>>& handles
) const {
    double_indexed_vertex_handle index_knot_vector1_level1(handle.vertex, handle.index, 1);
    double_indexed_vertex_handle index_knot_vector2_level1(handle.vertex, handle.index, 2);
    auto sum_knot_vectors1 = knot_vectors.at(index_knot_vector1_level1) + knot_vectors.at(index_knot_vector2_level1);

    auto wrapped_index = get_wrapped_offset_index(handle, -1);

    // Get face
    auto [h, q] = handles.at(handle);
    auto face_h = mesh.get_face_of_half_edge(h).expect(EXPECT_NO_BORDER);

    // Get neighbouring face
    indexed_vertex_handle neighbouring_index(handle.vertex, wrapped_index);
    auto [nh, nq] = handles.at(neighbouring_index);
    auto nface_h = mesh.get_face_of_half_edge(nh).expect(EXPECT_NO_BORDER);

    // Only calc the scale factor, if we are NOT at a t-joint
    float scale_factor = 1.0f;
    if (face_h != nface_h) {
        auto separating_edge = mesh.get_half_edge_between(face_h, nface_h).expect(EXPECT_NO_BORDER);
        scale_factor = fac(separating_edge);
    }

    double_indexed_vertex_handle index_knot_vector1_level2(handle.vertex, wrapped_index, 1);
    double_indexed_vertex_handle index_knot_vector2_level2(handle.vertex, wrapped_index, 2);
    auto sum_knot_vectors2 = knot_vectors.at(index_knot_vector1_level2) + knot_vectors.at(index_knot_vector2_level2);
    aa_rectangle r(vec2(0, 0), vec2(sum_knot_vectors1, sum_knot_vectors2 * scale_factor));

    return r;
}

regular_grid tmesh::get_grid(uint32_t resolution) const {
    regular_grid out(face_handle(0));

    // TODO: Implement

    return out;
}

vector<regular_grid> tmesh::get_grids(uint32_t resolution) const {
    vector<regular_grid> out;
    out.reserve(mesh.num_faces());

    for (auto&& fh: mesh.get_faces()) {
        // TODO: Get correct distance to extraordinary vertex and then skip faces below distance
        // Skip faces containing an extraordinary vertex
        vector<face_handle> faces_to_check;
//        mesh.get_neighbours_of_face(fh, faces_to_check);
        faces_to_check.push_back(fh);
        auto skip = false;
        for (auto&& cfh: faces_to_check) {
            for (auto&& vh: mesh.get_vertices_of_face(cfh)) {
                if (is_extraordinary(vh)) {
                    skip = true;
                }
            }
        }
        if (skip) {
            continue;
        }

        auto local_system_max = get_local_max_coordinates(fh);
        float u_coord = local_system_max.x;
        float v_coord = local_system_max.y;
        auto u_max = resolution + 1u;
        auto v_max = resolution + 1u;
        float step_u = u_coord / resolution;
        float step_v = v_coord / resolution;

        out.emplace_back(fh);
        auto& grid = out.back();
        grid.points.reserve(static_cast<size_t>(v_max));
        grid.num_points_x = u_max;
        grid.num_points_y = v_max;

        float current_u = 0;
        float current_v = 0;
        for (uint32_t v = 0; v < v_max; ++v) {
            current_u = 0;
            vector<vec3> row;
            row.reserve(static_cast<size_t>(u_max));
            for (uint32_t u = 0; u < u_max; ++u) {
                row.push_back(get_surface_point_of_face(min(current_u, u_coord), min(current_v, v_coord), fh));
                current_u += step_u;
            }
            grid.points.push_back(row);
            current_v += step_v;
        }
    }

    return out;
}

vec3 tmesh::get_surface_point_of_face(float u, float v, face_handle f) const {
    vec3 c(0.0f, 0.0f, 0.0f);
    float d = 0.0f;
    vec2 in(u, v);

    auto& supports = support_map[f];
    for (auto&& [index, trans]: supports) {
        // TODO: The knot vectors could be pre calculated
        auto [uv, vv] = get_knot_vectors(index);

        auto p = mesh.get_vertex_position(index.vertex);
        auto transformed = trans.apply(in);

        auto u_basis = tsplines::get_basis_fun(transformed.x, uv);
        auto v_basis = tsplines::get_basis_fun(transformed.y, vv);

        c += u_basis * v_basis * p;
        d += u_basis * v_basis;
    }

    return c / d;
}

float tsplines::get_basis_fun(float u, const array<float, 5>& knot_vector) {
    // Get the span in the knot_vector (the span is called i in the recurrence formula for b-splines)
    auto span = get_span(u, knot_vector);
    // Check if basis function is relevant
    if (!span) {
        return 0.0f;
    }

    // Unwrap the optional for shorter usage
    auto i = *span;

    float n00 = 0.0f;
    float n10 = 0.0f;
    float n20 = 0.0f;
    float n30 = 0.0f;

    // We use the recurrence formula for b-splines and assume a fixed degree of 3. The calculation triangle for
    // the basis functions is shown for each span, if we want to find N_0,3:
    switch (i) {
        case 0: {
            n00 = 1.0f;
            break;
        }
        case 1: {
            n10 = 1.0f;
            break;
        }
        case 2: {
            n20 = 1.0f;
            break;
        }
        case 3: {
            n30 = 1.0f;
            break;
        }
        default:
            panic("unhandled span in tsplines::get_basis_fun!");
    }

    // Calc N_0,3 using the set basis functions
    //
    //                   N_0,3
    //                  /     \
    //                 /       \
    //               N_0,2     N_1,2
    //               / \       /  \
    //              /   \     /    \
    //          N_0,1     N_1,1     N_2,1
    //            / \    / \       /  \
    //           /   \  /   \     /    \
    //       N_0,0   N_1,0   N_2,0    N_3,0
    //

    // Layer N_x,1
    float n01_left = ((u - knot_vector[0]) / (knot_vector[0+1] - knot_vector[0]));
    float n01_right = ((knot_vector[0+1+1] - u) / (knot_vector[0+1+1] - knot_vector[0+1]));
    float n01 = n01_left * n00 + n01_right * n10;

    float n11_left = ((u - knot_vector[1]) / (knot_vector[1+1] - knot_vector[1]));
    float n11_right = ((knot_vector[1+1+1] - u) / (knot_vector[1+1+1] - knot_vector[1+1]));
    float n11 = n11_left * n10 + n11_right * n20;

    float n21_left = ((u - knot_vector[2]) / (knot_vector[2+1] - knot_vector[2]));
    float n21_right = ((knot_vector[2+1+1] - u) / (knot_vector[2+1+1] - knot_vector[2+1]));
    float n21 = n21_left * n20 + n21_right * n30;

    // Layer N_x,2
    float n02_left = ((u - knot_vector[0]) / (knot_vector[0+2] - knot_vector[0]));
    float n02_right = ((knot_vector[0+2+1] - u) / (knot_vector[0+2+1] - knot_vector[0+1]));
    float n02 = n02_left * n01 + n02_right * n11;

    float n12_left = ((u - knot_vector[1]) / (knot_vector[1+2] - knot_vector[1]));
    float n12_right = ((knot_vector[1+2+1] - u) / (knot_vector[1+2+1] - knot_vector[1+1]));
    float n12 = n12_left * n11 + n12_right * n21;

    // Layer N_x,3
    float n03_left = ((u - knot_vector[0]) / (knot_vector[0+3] - knot_vector[0]));
    float n03_right = ((knot_vector[0+3+1] - u) / (knot_vector[0+3+1] - knot_vector[0+1]));
    float n03 = n03_left * n02 + n03_right * n12;

    return n03;
}

optional<uint8_t> tsplines::get_span(float u, const array<float, 5>& knot_vector) {
    if (u < knot_vector[0] || u >= knot_vector[4]) {
        return nullopt;
    }
    auto low = 0;
    auto high = 4;
    auto mid = static_cast<uint8_t>((low + high) / 2);
    while (u < knot_vector[mid] || u >= knot_vector[mid+1]) {
        if (u < knot_vector[mid]) {
            high = mid;
        } else {
            low = mid;
        }
        mid = static_cast<uint8_t>((low + high) / 2);
    }
    return mid;
}

pair<array<float, 5>, array<float, 5>> tmesh::get_knot_vectors(const indexed_vertex_handle& handle) const {

    // Note on variable names: a number i at the end means "domain + i" and a _i number means "domain - i"

    // Get half edge and face corresponding to the current handle
    auto [edge_h0, tag0] = handles.at(handle);
    auto face_h0 = mesh.get_face_of_half_edge(edge_h0).expect(EXPECT_NO_BORDER);

    // Get half edge and face handle for index + 1 cw
    auto index1 = get_wrapped_offset_index(handle, 1);
    indexed_vertex_handle handle1(handle.vertex, index1);
    auto [edge_h1, tag1] = handles.at(handle1);
    auto face_h1 = mesh.get_face_of_half_edge(edge_h1).expect(EXPECT_NO_BORDER);

    // Get half edge and face handle for index - 1 cw (which is + 1 ccw)
    auto index_1 = get_wrapped_offset_index(handle, -1);
    indexed_vertex_handle handle_1(handle.vertex, index_1);
    auto [edge_h_1, tag_1] = handles.at(handle_1);
    auto face_h_1 = mesh.get_face_of_half_edge(edge_h_1).expect(EXPECT_NO_BORDER);

    // Get half edge and face handle for index - 2 cw (which is + 2 ccw)
    auto index_2 = get_wrapped_offset_index(handle, -2);
    indexed_vertex_handle handle_2(handle.vertex, index_2);
    auto [edge_h_2, tag_2] = handles.at(handle_2);
    auto face_h_2 = mesh.get_face_of_half_edge(edge_h_2).expect(EXPECT_NO_BORDER);

    // Get two knot values for current handle
    auto knot01 = knot_vectors.at(double_indexed_vertex_handle(handle.vertex, handle.index, 1));
    auto knot02 = knot_vectors.at(double_indexed_vertex_handle(handle.vertex, handle.index, 2));

    // Get transformation from values from hindex + 1 into domain of handle
    float transform_01 = 1.0f;
    if (face_h0 != face_h1) {
        auto edge_bewteen_0_and1 = mesh.get_half_edge_between(face_h0, face_h1).expect(EXPECT_NO_BORDER);
        transform_01 = fac(edge_bewteen_0_and1);
    }

    // Get two knot values for handle with index + 1 and transform them into the domain of the current handle
    auto knot11 = knot_vectors.at(double_indexed_vertex_handle(handle.vertex, index1, 1)) * transform_01;
    auto knot12 = knot_vectors.at(double_indexed_vertex_handle(handle.vertex, index1, 2)) * transform_01;

    // Get transformation from values from hindex - 1 into domain of handle
    float transform_0_1 = 1.0f;
    if (face_h0 != face_h_1) {
        auto edge_bewteen_0_and_1 = mesh.get_half_edge_between(face_h0, face_h_1).expect(EXPECT_NO_BORDER);
        transform_0_1 = fac(edge_bewteen_0_and_1);
    }

    // Get two knot values for handle with index - 1 and transform them into the domain of the current handle
    auto knot_11 = knot_vectors.at(double_indexed_vertex_handle(handle.vertex, index_1, 1)) * transform_0_1;
    auto knot_12 = knot_vectors.at(double_indexed_vertex_handle(handle.vertex, index_1, 2)) * transform_0_1;

    // For the transition from index - 2 into domain of handle we need two transitions: -2 to -1 and -1 to handle
    // Get transformation from values from hindex - 2 into index - 1
    float transform__1_2 = 1.0f;
    if (face_h_1 != face_h_2) {
        auto edge_bewteen__1_and_2 = mesh.get_half_edge_between(face_h_1, face_h_2).expect(EXPECT_NO_BORDER);
        transform__1_2 = fac(edge_bewteen__1_and_2);
    }
    auto transform_0_2 = transform_0_1 * transform__1_2;

    // Get two knot values for handle with index - 2 and transform them into the domain of the current handle
    auto knot_21 = knot_vectors.at(double_indexed_vertex_handle(handle.vertex, index_2, 1)) * transform_0_2;
    auto knot_22 = knot_vectors.at(double_indexed_vertex_handle(handle.vertex, index_2, 2)) * transform_0_2;

    array<float, 5> u = {-knot_21 - knot_22, -knot_21, 0, knot01, knot01 + knot02};
    array<float, 5> v = {-knot11 - knot12, -knot11, 0, knot_11, knot_11 + knot_12};

    return {u, v};
}

uint32_t tmesh::get_wrapped_offset_index(const indexed_vertex_handle& handle, int32_t offset) const {
    auto valence = get_extended_valence(handle.vertex);
    return ((valence + handle.index) + offset) % valence;
}

uint32_t tmesh::get_extended_valence(const vertex_handle handle) const {
    auto edges = mesh.get_half_edges_of_vertex(handle, direction::ingoing);
    uint32_t valence = 0;
    for (auto&& e: edges) {
        valence += 1;
        if (!corners[e]) {
            valence += 1;
        }
    }
    return valence;
}

bool tmesh::is_extraordinary(const vertex_handle handle) const {
    return get_extended_valence(handle) != 4;
}

vec2 tmesh::get_local_max_coordinates(const face_handle& handle) const {
    auto inner_half_edges = mesh.get_half_edges_of_face(handle);
    vec2 out(0, 0);
    for (auto&& eh: inner_half_edges) {
        // Check u direction
        if (dir[eh] % 2 == 0) {
            out.x = max(out.x, uv[eh].x);
        } else {
            // Check v direction
            out.y = max(out.y, uv[eh].y);
        }
    }
    return out;
}

vec2 rotate(uint8_t times, const vec2& vec) {
    vec2 out(vec);
    switch (times % 4) {
        case 1: {
            auto swp = out.y;
            out.y = out.x;
            out.x = swp * -1.0f;
            break;
        }
        case 2:
            out *= -1.0f;
            break;
        case 3: {
            auto swp = out.y;
            out.y = out.x * -1.0f;
            out.x = swp;
            break;
        }
        default:
            // Catches case 0, where the input stays the same
            break;
    }
    return out;
}

bool indexed_vertex_handle::operator==(const indexed_vertex_handle& other) const {
    return vertex == other.vertex && index == other.index;
}

bool indexed_vertex_handle::operator!=(const indexed_vertex_handle& other) const {
    return vertex != other.vertex || index != other.index;
}

bool double_indexed_vertex_handle::operator==(const double_indexed_vertex_handle& other) const {
    return vertex == other.vertex && vertex_index == other.vertex_index && knot_index == other.knot_index;
}

bool double_indexed_vertex_handle::operator!=(const double_indexed_vertex_handle& other) const {
    return vertex != other.vertex || vertex_index != other.vertex_index || knot_index != other.knot_index;
}

vec2 transform::apply(const vec2& vec) const {
    return f * rotate(r, vec) + t;
}

transform transform::apply(const transform& trans) const {
    auto scale = f * trans.f;
    auto rotate = static_cast<uint8_t>((r + trans.r) % 4);
    auto translate = apply(trans.t);
    return transform(scale, rotate, translate);
}

}
