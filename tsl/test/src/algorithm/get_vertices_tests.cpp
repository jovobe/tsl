#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "tsl_tests/geometry/tmesh/tmesh_fixtures.hpp"
#include "tsl/algorithm/get_vertices.hpp"

namespace tsl_tests {

TEST_F(TmeshTestWithCubeData, GetRegularRingsAroundVertex) {
    // front side
    auto v0 = vertex_handles[0];
    auto v1 = vertex_handles[1];
    auto v2 = vertex_handles[2];
    auto v3 = vertex_handles[3];
    auto v4 = vertex_handles[4];
    auto v5 = vertex_handles[5];
    auto v6 = vertex_handles[6];
    auto v7 = vertex_handles[7];
    auto v8 = vertex_handles[8];

    // right side
    auto v17 = vertex_handles[17];
    auto v18 = vertex_handles[18];
    auto v21 = vertex_handles[21];
    auto v22 = vertex_handles[22];
    auto v23 = vertex_handles[23];
    auto v20 = vertex_handles[20];

    // top side
    auto v11 = vertex_handles[11];
    auto v15 = vertex_handles[15];
    auto v24 = vertex_handles[24];
    auto v9 = vertex_handles[9];

    auto vertices = get_regular_rings_around_vertex(mesh, v0, 2);
    ASSERT_THAT(vertices, ::testing::UnorderedElementsAre(
        v1, v2, v3, v4, v5, v6, v7, v8,
        v17, v18, v21, v22, v23, v20,
        v11, v15, v24, v9
    ));
}

TEST_F(TmeshTestWithCubeData, GetExtraordinaryVerticesInRegularRingsAroundVertex) {
    // front side
    auto v0 = vertex_handles[0];
    auto v4 = vertex_handles[4];
    auto v6 = vertex_handles[6];
    auto v8 = vertex_handles[8];

    // right side
    auto v17 = vertex_handles[17];
    auto v20 = vertex_handles[20];

    // top side
    auto v11 = vertex_handles[11];

    auto vertices = get_extraordinary_vertices_in_regular_rings_around_vertex(mesh, v0, 2);
    ASSERT_THAT(vertices, ::testing::UnorderedElementsAre(
        v4, v6, v8, v17, v20, v11
    ));
}

}
