#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tsl/geometry/half_edge_mesh.hpp>

using namespace tsl;

namespace tsl_tests {

class HalfEdgeMeshTestWithCubeData : public ::testing::Test {
protected:
    void SetUp() override {
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
        auto v0 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v2 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v3 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v0, v1, v2, v3}));
        vertex_handles.insert(vertex_handles.end(), {v0, v1, v2, v3});

        // f1
        auto v4 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v5 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v1, v4, v5, v2}));
        vertex_handles.insert(vertex_handles.end(), {v4, v5});

        // f2
        auto v6 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v7 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v3, v2, v7, v6}));
        vertex_handles.insert(vertex_handles.end(), {v6, v7});

        // f3
        auto v8 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
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
        auto v9 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v10 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v4, v9, v10, v5}));
        vertex_handles.insert(vertex_handles.end(), {v9, v10});

        // f5
        auto v11 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v12 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v9, v11, v12, v10}));
        vertex_handles.insert(vertex_handles.end(), {v11, v12});

        // f6
        auto v13 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v5, v10, v13, v8}));
        vertex_handles.insert(vertex_handles.end(), {v13});

        // f7
        auto v14 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
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
        auto v15 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v16 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v11, v15, v16, v12}));
        vertex_handles.insert(vertex_handles.end(), {v15, v16});

        // f9
        auto v17 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v18 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v15, v17, v18, v16}));
        vertex_handles.insert(vertex_handles.end(), {v17, v18});

        // f10
        auto v19 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v12, v16, v19, v14}));
        vertex_handles.insert(vertex_handles.end(), {v19});

        // f11
        auto v20 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
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
        auto v21 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        auto v22 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v17, v21, v22, v18}));
        vertex_handles.insert(vertex_handles.end(), {v21, v22});

        // f13
        face_handles.push_back(mesh.add_face({v21, v0, v3, v22}));

        // f14
        auto v23 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
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
        auto v24 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
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
        auto v25 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
        face_handles.push_back(mesh.add_face({v6, v7, v25, v23}));
        vertex_handles.insert(vertex_handles.end(), {v25});

        // f21
        face_handles.push_back(mesh.add_face({v7, v8, v13, v25}));

        // f22
        face_handles.push_back(mesh.add_face({v23, v25, v19, v20}));

        // f23
        face_handles.push_back(mesh.add_face({v25, v13, v14, v19}));
    }

    half_edge_mesh mesh;
    vector<vertex_handle> vertex_handles;
    vector<face_handle> face_handles;
};

class HalfEdgeMeshTest : public ::testing::Test {
protected:
    half_edge_mesh mesh;
};

TEST_F(HalfEdgeMeshTest, IsFaceInsertionValid) {
    auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v2 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v3 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v4 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    EXPECT_FALSE(mesh.is_face_insertion_valid({}));
    EXPECT_FALSE(mesh.is_face_insertion_valid({v1, v2, v3}));
    EXPECT_TRUE(mesh.is_face_insertion_valid({v1, v2, v3, v4}));
}

TEST_F(HalfEdgeMeshTest, IsFaceInsertionValidSameVertices) {
    auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v2 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v3 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v4 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    EXPECT_TRUE(mesh.is_face_insertion_valid({v1, v2, v3, v4}));

    // Add the following face (f1):
    //
    //       (v2) ====== (v1)
    //        ||          ||
    //        ||          ||
    //        ||    f1    ||
    //        ||          ||
    //       (v3) ====== (v4)
    mesh.add_face({v1, v2, v3, v4});

    EXPECT_FALSE(mesh.is_face_insertion_valid({v1, v2, v3, v4}));
    EXPECT_FALSE(mesh.is_face_insertion_valid({v2, v1, v4, v3}));
    EXPECT_FALSE(mesh.is_face_insertion_valid({v1, v1, v3, v4}));
    EXPECT_FALSE(mesh.is_face_insertion_valid({v1, v1, v1, v1}));
}

TEST_F(HalfEdgeMeshTest, GetVertexPosition) {
    auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v2 = mesh.add_vertex({1.0f, 2.0f, 3.0f});

    {
        const auto& mesh_const = mesh;
        auto v1_pos = mesh_const.get_vertex_position(v1);
        EXPECT_EQ(vec3(0.0f, 0.0f, 0.0f), v1_pos);
    }

    auto& v2_pos = mesh.get_vertex_position(v2);
    EXPECT_EQ(vec3(1.0f, 2.0f, 3.0f), v2_pos);

    v2_pos.x = 42.0f;
    EXPECT_EQ(vec3(42.0f, 2.0f, 3.0f), v2_pos);
}

TEST_F(HalfEdgeMeshTest, VertexIterator) {
    auto v0 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v2 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v3 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    vector<vertex_handle> expected_handles;
    expected_handles.insert(expected_handles.begin(), {v0, v1, v2, v3});
    vector<vertex_handle> vertex_handles;
    for (auto&& vh: mesh.get_vertices()) {
        vertex_handles.push_back(vh);
    }

    ASSERT_THAT(vertex_handles, ::testing::UnorderedElementsAreArray(expected_handles));
}

TEST_F(HalfEdgeMeshTest, GetVerticesOfFace) {
    auto v0 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v2 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v3 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    auto v4 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v5 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    vector<face_handle> face_handles;

    // Add the following face (f0):
    //
    //       (v1) ====== (v0)
    //        ||          ||
    //        ||          ||
    //        ||    f0    ||
    //        ||          ||
    //       (v2) ====== (v3)
    face_handles.push_back(mesh.add_face({v0, v1, v2, v3}));

    ASSERT_THAT(mesh.get_vertices_of_face(face_handles[0]), ::testing::UnorderedElementsAre(v0, v1, v2, v3));

    // Add the following face (f1):
    //
    //       (v4) ====== (v1) ====== (v0)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f1    ||    f0    ||
    //        ||          ||          ||
    //       (v5) ====== (v2) ====== (v3)
    face_handles.push_back(mesh.add_face({v1, v4, v5, v2}));

    ASSERT_THAT(mesh.get_vertices_of_face(face_handles[0]), ::testing::UnorderedElementsAre(v0, v1, v2, v3));
    ASSERT_THAT(mesh.get_vertices_of_face(face_handles[1]), ::testing::UnorderedElementsAre(v1, v2, v4, v5));
}

// TODO: Implement orientability check in `half_edge_mesh::add_face()`
TEST_F(HalfEdgeMeshTest, Orientability) {
    auto v0 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v2 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v3 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    auto v4 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v5 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    mesh.add_face({v0, v1, v2, v3});

    // This should panic, because it will break the orientability of the mesh
//    ASSERT_THROW(mesh.add_face({v1, v2, v4, v5}), panic_exception);
}

TEST_F(HalfEdgeMeshTest, FaceIterator) {
    auto v0 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v2 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v3 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    auto v4 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v5 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v6 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v7 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    // Create the following mesh:
    //
    //       (v4) ====== (v1) ====== (v0)
    //        ||          ||          ||
    //        ||          ||          ||
    //        ||    f1    ||    f0    ||
    //        ||          ||          ||
    //       (v5) ====== (v2) ====== (v3)
    //                    ||          ||
    //                    ||          ||
    //                    ||    f2    ||
    //                    ||          ||
    //                   (v6) ====== (v7)
    vector<face_handle> expected_handles;
    expected_handles.push_back(mesh.add_face({v0, v1, v2, v3}));
    expected_handles.push_back(mesh.add_face({v1, v4, v5, v2}));
    expected_handles.push_back(mesh.add_face({v3, v2, v6, v7}));

    EXPECT_EQ(3, mesh.num_faces());

    vector<face_handle> face_handles;
    for (auto&& fh: mesh.get_faces()) {
        face_handles.push_back(fh);
    }

    ASSERT_THAT(face_handles, ::testing::UnorderedElementsAreArray(expected_handles));
}

TEST_F(HalfEdgeMeshTest, EdgeIterator) {
    auto v0 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v2 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v3 = mesh.add_vertex({0.0f, 0.0f, 0.0f});

    // Add the following face (f0):
    //
    //       (v1) ====== (v0)
    //        ||          ||
    //        ||          ||
    //        ||    f0    ||
    //        ||          ||
    //       (v2) ====== (v3)
    mesh.add_face({v0, v1, v2, v3});
    vector<vertex_handle> expected_handles;
    expected_handles.insert(expected_handles.begin(), {v0, v0, v1, v1, v2, v2, v3, v3});

    vector<vertex_handle> vertex_handles;
    for (auto&& eh: mesh.get_half_edges()) {
        vertex_handles.push_back(mesh.get_vertices_of_half_edge(eh)[0]);
    }

    ASSERT_THAT(vertex_handles, ::testing::UnorderedElementsAreArray(expected_handles));
}

TEST_F(HalfEdgeMeshTestWithCubeData, IsFaceInsertionValidNonManifoldTripleEdge) {
    // Create non manifold triple edge
    auto vte0 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto vte1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    auto v0 = vertex_handles[0];
    auto v3 = vertex_handles[3];

    ASSERT_THROW(mesh.add_face({v0, v3, vte0, vte1}), panic_exception);
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetVerticesOfFace) {
    auto vertices = mesh.get_vertices_of_face(face_handles[0]);
    auto v0 = vertex_handles[0];
    auto v1 = vertex_handles[1];
    auto v2 = vertex_handles[2];
    auto v3 = vertex_handles[3];
    ASSERT_THAT(vertices, ::testing::UnorderedElementsAre(v0, v1, v2, v3));
}

TEST_F(HalfEdgeMeshTestWithCubeData, NumFacesEdgesVertices) {
    EXPECT_EQ(24, mesh.num_faces());
    EXPECT_EQ(26, mesh.num_vertices());
    EXPECT_EQ(48, mesh.num_edges());
    EXPECT_EQ(96, mesh.num_half_edges());
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetVerticesOfEdgeAndGetEdgeBetween) {
    auto v2 = vertex_handles[2];
    auto v3 = vertex_handles[3];
    auto edge_handle = mesh.get_edge_between(v2, v3);
    auto vertices = mesh.get_vertices_of_edge(edge_handle.unwrap());
    ASSERT_THAT(vertices, ::testing::UnorderedElementsAre(v2, v3));
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetHalfEdgeBetween) {
    auto v2 = vertex_handles[2];
    auto v3 = vertex_handles[3];
    auto edge_handle = mesh.get_half_edge_between(v2, v3);
    auto vertices = mesh.get_vertices_of_half_edge(edge_handle.unwrap());
    ASSERT_THAT(vertices, ::testing::UnorderedElementsAre(v2, v3));
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetFacesOfEdge) {
    auto v2 = vertex_handles[2];
    auto v3 = vertex_handles[3];
    auto f0 = optional_face_handle(face_handles[0]);
    auto f2 = optional_face_handle(face_handles[2]);
    auto edge_handle = mesh.get_edge_between(v2, v3);
    auto faces = mesh.get_faces_of_edge(edge_handle.unwrap());
    ASSERT_THAT(faces, ::testing::UnorderedElementsAre(f0, f2));
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetEdgesOfVertexFourEdges) {
    auto v2 = vertex_handles[2];
    auto edges = mesh.get_edges_of_vertex(v2);
    EXPECT_EQ(4, edges.size());

    auto v1 = vertex_handles[1];
    auto v5 = vertex_handles[5];
    auto v7 = vertex_handles[7];
    auto v3 = vertex_handles[3];

    auto edge_21 = mesh.get_edge_between(v2, v1).unwrap();
    auto edge_25 = mesh.get_edge_between(v2, v5).unwrap();
    auto edge_27 = mesh.get_edge_between(v2, v7).unwrap();
    auto edge_23 = mesh.get_edge_between(v2, v3).unwrap();

    ASSERT_THAT(edges, ::testing::UnorderedElementsAre(edge_21, edge_23, edge_25, edge_27));
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetHalfEdgesOfVertexFourEdges) {
    auto v2 = vertex_handles[2];
    auto edges = mesh.get_half_edges_of_vertex(v2);
    EXPECT_EQ(4, edges.size());

    auto v1 = vertex_handles[1];
    auto v5 = vertex_handles[5];
    auto v7 = vertex_handles[7];
    auto v3 = vertex_handles[3];

    auto edge_21 = mesh.get_half_edge_between(v1, v2).unwrap();
    auto edge_25 = mesh.get_half_edge_between(v5, v2).unwrap();
    auto edge_27 = mesh.get_half_edge_between(v7, v2).unwrap();
    auto edge_23 = mesh.get_half_edge_between(v3, v2).unwrap();

    ASSERT_THAT(edges, ::testing::UnorderedElementsAre(edge_21, edge_23, edge_25, edge_27));
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetEdgesOfVertexThreeEdges) {
    auto v0 = vertex_handles[0];
    auto edges = mesh.get_edges_of_vertex(v0);
    EXPECT_EQ(3, edges.size());

    auto v1 = vertex_handles[1];
    auto v3 = vertex_handles[3];
    auto v21 = vertex_handles[21];

    auto edge_01 = mesh.get_edge_between(v0, v1).unwrap();
    auto edge_03 = mesh.get_edge_between(v0, v3).unwrap();
    auto edge_021 = mesh.get_edge_between(v0, v21).unwrap();

    ASSERT_THAT(edges, ::testing::UnorderedElementsAre(edge_01, edge_03, edge_021));
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetHalfEdgesOfVertexThreeEdges) {
    auto v0 = vertex_handles[0];
    auto edges = mesh.get_half_edges_of_vertex(v0);
    EXPECT_EQ(3, edges.size());

    auto v1 = vertex_handles[1];
    auto v3 = vertex_handles[3];
    auto v21 = vertex_handles[21];

    auto edge_01 = mesh.get_half_edge_between(v1, v0).unwrap();
    auto edge_03 = mesh.get_half_edge_between(v3, v0).unwrap();
    auto edge_021 = mesh.get_half_edge_between(v21, v0).unwrap();

    ASSERT_THAT(edges, ::testing::UnorderedElementsAre(edge_01, edge_03, edge_021));
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetFaceBetween) {
    auto v1 = vertex_handles[1];
    auto v4 = vertex_handles[4];
    auto v5 = vertex_handles[5];
    auto v2 = vertex_handles[2];
    auto f1 = face_handles[1];
    EXPECT_EQ(f1, mesh.get_face_between({v1, v4, v5, v2}).unwrap());
    EXPECT_EQ(f1, mesh.get_face_between({v2, v5, v4, v1}).unwrap());
    EXPECT_EQ(f1, mesh.get_face_between({v2, v1, v4, v5}).unwrap());

    // This should throw an exception, because the order of the vertices is messed up
    ASSERT_THROW(mesh.get_face_between({v1, v5, v4, v2}).unwrap(), panic_exception);
}

TEST_F(HalfEdgeMeshTestWithCubeData, NumAdjacentFaces) {
    auto v2 = vertex_handles[2];
    auto v3 = vertex_handles[3];
    auto edge_handle = mesh.get_edge_between(v2, v3);
    EXPECT_EQ(2, mesh.num_adjacent_faces(edge_handle.unwrap()));
}

TEST_F(HalfEdgeMeshTestWithCubeData, GetEdgesOfFace) {
    auto f0 = face_handles[0];
    auto half_edges = mesh.get_half_edges_of_face(f0);
    EXPECT_EQ(4, half_edges.size());

    vector<half_edge_handle> expected_handles;
    expected_handles.push_back(mesh.get_half_edge_between(vertex_handles[0], vertex_handles[1]).unwrap());
    expected_handles.push_back(mesh.get_half_edge_between(vertex_handles[1], vertex_handles[2]).unwrap());
    expected_handles.push_back(mesh.get_half_edge_between(vertex_handles[2], vertex_handles[3]).unwrap());
    expected_handles.push_back(mesh.get_half_edge_between(vertex_handles[3], vertex_handles[0]).unwrap());

    ASSERT_THAT(half_edges, ::testing::UnorderedElementsAreArray(expected_handles));
}

}

#pragma clang diagnostic pop
