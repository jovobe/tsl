#include <gtest/gtest.h>

#include "tsl/geometry/transform.hpp"

using namespace tsl;

namespace tsl_tests {

TEST(RotateTest, Zero) {
    vec2 vec(0, 0);
    for (uint8_t i = 0; i < 10; ++i) {
        EXPECT_EQ(vec, rotate(i, vec));
    }
}

TEST(RotateTest, Values1) {
    vec2 vec(1, 0);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(0,  1), rotate(1, vec));
    EXPECT_EQ(vec2(-1, 0), rotate(2, vec));
    EXPECT_EQ(vec2(0, -1), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values2) {
    vec2 vec(0, 1);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(-1, 0), rotate(1, vec));
    EXPECT_EQ(vec2(0, -1), rotate(2, vec));
    EXPECT_EQ(vec2(1,  0), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values3) {
    vec2 vec(1, 1);
    EXPECT_EQ(vec,                rotate(0, vec));
    EXPECT_EQ(vec2(-1,  1), rotate(1, vec));
    EXPECT_EQ(vec2(-1, -1), rotate(2, vec));
    EXPECT_EQ(vec2(1,  -1), rotate(3, vec));
    EXPECT_EQ(vec,                rotate(4, vec));
}

TEST(RotateTest, Values4) {
    vec2 vec(-1, 0);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(0, -1), rotate(1, vec));
    EXPECT_EQ(vec2(1,  0), rotate(2, vec));
    EXPECT_EQ(vec2(0,  1), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values5) {
    vec2 vec(0, -1);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(1,  0), rotate(1, vec));
    EXPECT_EQ(vec2(0,  1), rotate(2, vec));
    EXPECT_EQ(vec2(-1, 0), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values6) {
    vec2 vec(-1, -1);
    EXPECT_EQ(vec,               rotate(0, vec));
    EXPECT_EQ(vec2(1, -1), rotate(1, vec));
    EXPECT_EQ(vec2(1,  1), rotate(2, vec));
    EXPECT_EQ(vec2(-1, 1), rotate(3, vec));
    EXPECT_EQ(vec,               rotate(4, vec));
}

TEST(RotateTest, Values7) {
    vec2 vec(-1, 1);
    EXPECT_EQ(vec,                rotate(0, vec));
    EXPECT_EQ(vec2(-1, -1), rotate(1, vec));
    EXPECT_EQ(vec2(1,  -1), rotate(2, vec));
    EXPECT_EQ(vec2(1,   1), rotate(3, vec));
    EXPECT_EQ(vec,                rotate(4, vec));
}

TEST(RotateTest, Values8) {
    vec2 vec(1, -1);
    EXPECT_EQ(vec,                rotate(0, vec));
    EXPECT_EQ(vec2(1,   1), rotate(1, vec));
    EXPECT_EQ(vec2(-1,  1), rotate(2, vec));
    EXPECT_EQ(vec2(-1, -1), rotate(3, vec));
    EXPECT_EQ(vec,                rotate(4, vec));
}

class TransformTest : public ::testing::Test {
protected:
    TransformTest() : t1(1, 0, vec2(1, 1)), t2(0.5, 1, vec2(0, 0)), t3(2, 3, vec2(4, -5)), v(2, 3) {}

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
    EXPECT_EQ(vec2(-1.5, 1), t2.apply(v));
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

}
