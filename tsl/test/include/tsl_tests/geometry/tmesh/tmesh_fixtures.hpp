#ifndef TEST_INCLUDE_TSL_TESTS_GEOMETRY_TMESH_TMESH_FIXTURES_HPP
#define TEST_INCLUDE_TSL_TESTS_GEOMETRY_TMESH_TMESH_FIXTURES_HPP

#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "tsl/geometry/tmesh/tmesh.hpp"
#include "tsl/geometry/tmesh/handles.hpp"

using namespace tsl;

namespace tsl_tests {

class TmeshTestWithCubeData : public ::testing::Test {
protected:
    void SetUp() override;

    tmesh mesh;
    vector<vertex_handle> vertex_handles;
    vector<face_handle> face_handles;
};

class TmeshTestWithTfaceTest : public ::testing::Test {
protected:
    void SetUp() override;

    tmesh mesh;
    vector<vertex_handle> vertex_handles;
    vector<face_handle> face_handles;
};

class TmeshTestAsGrid : public ::testing::Test {
protected:
    void SetUp() override;

    tmesh mesh;
    vector<vertex_handle> vertex_handles;
    vector<face_handle> face_handles;
};

}

#endif //TEST_INCLUDE_TSL_TESTS_GEOMETRY_TMESH_TMESH_FIXTURES_HPP
