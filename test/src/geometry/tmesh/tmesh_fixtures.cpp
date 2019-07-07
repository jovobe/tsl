#include "tsl_tests/geometry/tmesh/tmesh_fixtures.hpp"

namespace tsl_tests {

void TmeshTestWithCubeData::SetUp() {
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
    //        ||          ||          ||
    //       (v8) ====== (v7) ====== (v6)

    // f0
    auto v0 = mesh.add_vertex({0, 0, 0});
    auto v1 = mesh.add_vertex({0, 0, 0});
    auto v2 = mesh.add_vertex({0, 0, 0});
    auto v3 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v0, v1, v2, v3}));
    vertex_handles.insert(vertex_handles.end(), {v0, v1, v2, v3});

    // f1
    auto v4 = mesh.add_vertex({0, 0, 0});
    auto v5 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v1, v4, v5, v2}));
    vertex_handles.insert(vertex_handles.end(), {v4, v5});

    // f2
    auto v6 = mesh.add_vertex({0, 0, 0});
    auto v7 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v3, v2, v7, v6}));
    vertex_handles.insert(vertex_handles.end(), {v6, v7});

    // f3
    auto v8 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v2, v5, v8, v7}));
    vertex_handles.insert(vertex_handles.end(), {v8});

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
    //        ||          ||          ||
    //       (v14) ===== (v13) ===== (v8)

    // f4
    auto v9 = mesh.add_vertex({0, 0, 0});
    auto v10 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v4, v9, v10, v5}));
    vertex_handles.insert(vertex_handles.end(), {v9, v10});

    // f5
    auto v11 = mesh.add_vertex({0, 0, 0});
    auto v12 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v9, v11, v12, v10}));
    vertex_handles.insert(vertex_handles.end(), {v11, v12});

    // f6
    auto v13 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v5, v10, v13, v8}));
    vertex_handles.insert(vertex_handles.end(), {v13});

    // f7
    auto v14 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v10, v12, v14, v13}));
    vertex_handles.insert(vertex_handles.end(), {v14});

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
    //        ||          ||          ||
    //       (v20) ===== (v19) ===== (v14)

    // f8
    auto v15 = mesh.add_vertex({0, 0, 0});
    auto v16 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v11, v15, v16, v12}));
    vertex_handles.insert(vertex_handles.end(), {v15, v16});

    // f9
    auto v17 = mesh.add_vertex({0, 0, 0});
    auto v18 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v15, v17, v18, v16}));
    vertex_handles.insert(vertex_handles.end(), {v17, v18});

    // f10
    auto v19 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v12, v16, v19, v14}));
    vertex_handles.insert(vertex_handles.end(), {v19});

    // f11
    auto v20 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v16, v18, v20, v19}));
    vertex_handles.insert(vertex_handles.end(), {v20});

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
    //        ||          ||          ||
    //       (v6) ====== (v23) ===== (v20)

    // f12
    auto v21 = mesh.add_vertex({0, 0, 0});
    auto v22 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v17, v21, v22, v18}));
    vertex_handles.insert(vertex_handles.end(), {v21, v22});

    // f13
    face_handles.push_back(mesh.add_face({v21, v0, v3, v22}));

    // f14
    auto v23 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v18, v22, v23, v20}));
    vertex_handles.insert(vertex_handles.end(), {v23});

    // f15
    face_handles.push_back(mesh.add_face({v22, v3, v6, v23}));

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
    auto v24 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v17, v15, v24, v21}));
    vertex_handles.insert(vertex_handles.end(), {v24});

    // f17
    face_handles.push_back(mesh.add_face({v15, v11, v9, v24}));

    // f18
    face_handles.push_back(mesh.add_face({v21, v24, v1, v0}));

    // f19
    face_handles.push_back(mesh.add_face({v24, v9, v4, v1}));

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
    auto v25 = mesh.add_vertex({0, 0, 0});
    face_handles.push_back(mesh.add_face({v6, v7, v25, v23}));
    vertex_handles.insert(vertex_handles.end(), {v25});

    // f21
    face_handles.push_back(mesh.add_face({v7, v8, v13, v25}));

    // f22
    face_handles.push_back(mesh.add_face({v23, v25, v19, v20}));

    // f23
    face_handles.push_back(mesh.add_face({v25, v13, v14, v19}));
}

void TmeshTestWithTfaceTest::SetUp() {
    // Create tmesh with t face:
    //         v0 ==== v1 ==== v2
    //         ||      ||      ||
    //         ||  f0  ||  f1  ||
    //         ||      ||      ||
    //         v3 ==== v4 ==== v5
    //         ||              ||
    //         ||      f2      ||
    //         ||              ||
    //         v6 ============ v7

    auto& hem = mesh;
    vertex_handles.push_back(hem.add_vertex(vec3(0, 0, 0)));
    vertex_handles.push_back(hem.add_vertex(vec3(0, 0, 0)));
    vertex_handles.push_back(hem.add_vertex(vec3(0, 0, 0)));
    vertex_handles.push_back(hem.add_vertex(vec3(0, 0, 0)));
    vertex_handles.push_back(hem.add_vertex(vec3(0, 0, 0)));
    vertex_handles.push_back(hem.add_vertex(vec3(0, 0, 0)));
    vertex_handles.push_back(hem.add_vertex(vec3(0, 0, 0)));
    vertex_handles.push_back(hem.add_vertex(vec3(0, 0, 0)));

    face_handles.push_back(hem.add_face({vertex_handles[1], vertex_handles[0], vertex_handles[3], vertex_handles[4]}));
    face_handles.push_back(hem.add_face({vertex_handles[2], vertex_handles[1], vertex_handles[4], vertex_handles[5]}));
    face_handles.push_back(hem.add_face({
                                        new_face_vertex(vertex_handles[5]),
                                        new_face_vertex(vertex_handles[4], false, 1.0),
                                        new_face_vertex(vertex_handles[3]),
                                        new_face_vertex(vertex_handles[6]),
                                        new_face_vertex(vertex_handles[7])
                                        }));
}

}
