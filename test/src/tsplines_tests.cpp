#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>

#include <tsl/tsplines.hpp>

using namespace tsl;

namespace tsl_tests {

TEST(RotateTest, Zero) {
    vec2 vec(0.0f, 0.0f);
    for (uint8_t i = 0; i < 10; ++i) {
        EXPECT_EQ(vec, rotate(i, vec));
    }
}

TEST(RotateTest, Values1) {
    vec2 vec(1.0f, 0.0f);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(0.0f,  1.0f), rotate(1, vec));
    EXPECT_EQ(vec2(-1.0f, 0.0f), rotate(2, vec));
    EXPECT_EQ(vec2(0.0f, -1.0f), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values2) {
    vec2 vec(0.0f, 1.0f);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(-1.0f, 0.0f), rotate(1, vec));
    EXPECT_EQ(vec2(0.0f, -1.0f), rotate(2, vec));
    EXPECT_EQ(vec2(1.0f,  0.0f), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values3) {
    vec2 vec(1.0f, 1.0f);
    EXPECT_EQ(vec,                rotate(0, vec));
    EXPECT_EQ(vec2(-1.0f,  1.0f), rotate(1, vec));
    EXPECT_EQ(vec2(-1.0f, -1.0f), rotate(2, vec));
    EXPECT_EQ(vec2(1.0f,  -1.0f), rotate(3, vec));
    EXPECT_EQ(vec,                rotate(4, vec));
}

TEST(RotateTest, Values4) {
    vec2 vec(-1.0f, 0.0f);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(0.0f, -1.0f), rotate(1, vec));
    EXPECT_EQ(vec2(1.0f,  0.0f), rotate(2, vec));
    EXPECT_EQ(vec2(0.0f,  1.0f), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values5) {
    vec2 vec(0.0f, -1.0f);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(1.0f,  0.0f), rotate(1, vec));
    EXPECT_EQ(vec2(0.0f,  1.0f), rotate(2, vec));
    EXPECT_EQ(vec2(-1.0f, 0.0f), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values6) {
    vec2 vec(-1.0f, -1.0f);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(1.0f, -1.0f), rotate(1, vec));
    EXPECT_EQ(vec2(1.0f,  1.0f), rotate(2, vec));
    EXPECT_EQ(vec2(-1.0f, 1.0f), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values7) {
    vec2 vec(-1.0f, 1.0f);
    EXPECT_EQ(vec,                rotate(0, vec));
    EXPECT_EQ(vec2(-1.0f, -1.0f), rotate(1, vec));
    EXPECT_EQ(vec2(1.0f,  -1.0f), rotate(2, vec));
    EXPECT_EQ(vec2(1.0f,   1.0f), rotate(3, vec));
    EXPECT_EQ(vec,                rotate(4, vec));
}

TEST(RotateTest, Values8) {
    vec2 vec(1.0f, -1.0f);
    EXPECT_EQ(vec,                rotate(0, vec));
    EXPECT_EQ(vec2(1.0f,   1.0f), rotate(1, vec));
    EXPECT_EQ(vec2(-1.0f,  1.0f), rotate(2, vec));
    EXPECT_EQ(vec2(-1.0f, -1.0f), rotate(3, vec));
    EXPECT_EQ(vec,                rotate(4, vec));
}

class TransformTest : public ::testing::Test {
protected:
    TransformTest() : t1(1, 0, vec2(1, 1)), t2(0.5f, 1, vec2(0, 0)), t3(2, 3, vec2(4, -5)), v(2, 3) {}

    // 1 * R^0 + (1, 1)
    transform t1;

    // 0.5 * R^1 + (0, 0)
    transform t2;

    // 2 * R^3 + (4, -5)
    transform t3;

    // (2, 3)
    vec2 v;
};

TEST_F(TransformTest, ApplyVector) {
    EXPECT_EQ(vec2(3, 4), t1.apply(v));
    EXPECT_EQ(vec2(-1.5f, 1), t2.apply(v));
    EXPECT_EQ(vec2(10, -9), t3.apply(v));
}

TEST_F(TransformTest, ApplyTransform) {
    EXPECT_EQ(vec2(-.5, 2), t1.apply(t2).apply(v));
    EXPECT_EQ(vec2(-2, 1.5), t2.apply(t1).apply(v)); // <--
    EXPECT_EQ(t2.apply(t1.apply(v)), t2.apply(t1).apply(v));
    EXPECT_EQ(vec2(6, -2), t3.apply(t2).apply(v));
    EXPECT_EQ(vec2(4.5, 5), t2.apply(t3).apply(v)); // <--
    EXPECT_EQ(t2.apply(t3.apply(v)), t2.apply(t3).apply(v));
    EXPECT_EQ(vec2(11, -8), t1.apply(t3).apply(v));
    EXPECT_EQ(vec2(12, -11), t3.apply(t1).apply(v)); // <--
    EXPECT_EQ(t3.apply(t1.apply(v)), t3.apply(t1).apply(v));
}

class TmeshTest : public ::testing::Test {
protected:
    TmeshTest() : mesh(tsplines::get_example_data_1()) {}
    tmesh mesh;
};

TEST_F(TmeshTest, DetermineLocalCoordinateSystems) {
    // TODO: Do real testing here!
    auto data = tsplines::get_example_data_1();
    auto grid = data.get_grid(10);
}

class TmeshWithTfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create half edge mesh with t face:
        //         v0 ==== v1 ==== v2
        //         ||      ||      ||
        //         ||  f0  ||  f1  ||
        //         ||      ||      ||
        //         v3 ==== v4 ==== v5
        //         ||              ||
        //         ||      f2      ||
        //         ||              ||
        //         v6 ============ v7

        auto& hem = mesh.mesh;
        vertex_handles.push_back(hem.add_vertex(vec3(0.0f, 0.0f, 0.0f)));
        vertex_handles.push_back(hem.add_vertex(vec3(0.0f, 0.0f, 0.0f)));
        vertex_handles.push_back(hem.add_vertex(vec3(0.0f, 0.0f, 0.0f)));
        vertex_handles.push_back(hem.add_vertex(vec3(0.0f, 0.0f, 0.0f)));
        vertex_handles.push_back(hem.add_vertex(vec3(0.0f, 0.0f, 0.0f)));
        vertex_handles.push_back(hem.add_vertex(vec3(0.0f, 0.0f, 0.0f)));
        vertex_handles.push_back(hem.add_vertex(vec3(0.0f, 0.0f, 0.0f)));
        vertex_handles.push_back(hem.add_vertex(vec3(0.0f, 0.0f, 0.0f)));

        face_handles.push_back(hem.add_face({vertex_handles[1], vertex_handles[0], vertex_handles[3], vertex_handles[4]}));
        face_handles.push_back(hem.add_face({vertex_handles[2], vertex_handles[1], vertex_handles[4], vertex_handles[5]}));
        face_handles.push_back(hem.add_face({vertex_handles[5], vertex_handles[4], vertex_handles[3], vertex_handles[6], vertex_handles[7]}));

        mesh.corners = dense_half_edge_map<bool>(true);

        // Get half edge between v5 and v4 which is connected to f2
        auto he = hem.get_half_edge_between(vertex_handles[5], vertex_handles[4]).unwrap();
        mesh.corners[he] = false;
    }

    tmesh mesh;
    vector<vertex_handle> vertex_handles;
    vector<face_handle> face_handles;
};

TEST_F(TmeshWithTfaceTest, GetExtendedValence) {
    EXPECT_EQ(4, mesh.get_extended_valence(vertex_handles[4]));
}

TEST(GetSpanTest, DefaultKnotVector) {
    array<float, 5> knot_vector = {-2, -1, 0, 1, 2};

    EXPECT_EQ(0, *tsplines::get_span(-2, knot_vector));
    EXPECT_EQ(0, *tsplines::get_span(-1.5, knot_vector));
    EXPECT_EQ(1, *tsplines::get_span(-1, knot_vector));
    EXPECT_EQ(1, *tsplines::get_span(-.5, knot_vector));
    EXPECT_EQ(2, *tsplines::get_span(0, knot_vector));
    EXPECT_EQ(2, *tsplines::get_span(.5, knot_vector));
    EXPECT_EQ(3, *tsplines::get_span(1, knot_vector));
    EXPECT_EQ(3, *tsplines::get_span(1.5, knot_vector));

    EXPECT_EQ(nullopt, tsplines::get_span(2, knot_vector));
    EXPECT_EQ(nullopt, tsplines::get_span(-2.1, knot_vector));
    EXPECT_EQ(nullopt, tsplines::get_span(-5, knot_vector));
    EXPECT_EQ(nullopt, tsplines::get_span(2.1, knot_vector));
}

}

#pragma clang diagnostic pop
