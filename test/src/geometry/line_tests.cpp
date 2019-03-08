#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <glm/glm.hpp>

#include <tsl/geometry/line.hpp>

using glm::vec2;

using namespace tsl;

namespace tsl_tests {

class LineTest : public ::testing::Test {
protected:
    LineTest() : l1(vec2(0, 0), vec2(3, 3)), l2(vec2(0, 0), vec2(1, 0)), l3(vec2(0, 0), vec2(0, 1)) {}

    // Diagonal line
    line l1;

    // Line x-axis alined
    line l2;

    // Line y-axis alined
    line l3;
};

TEST_F(LineTest, Intersects) {
    // line in rect
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(0, 0), vec2(1, 1))));
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(-0.5f, -0.5f), vec2(5, 5))));

    // line through rect
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(0.5f, 0.5f), vec2(1, 1))));

    // line into recht
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(0.5f, 0.5f), vec2(5, 5))));

    // line out from rect
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(-0.5f, -0.5f), vec2(1, 1))));

    // line on rect edge
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(0, 0), vec2(1, 1))));

    // line above rect
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(-1, -1), vec2(-1, -0.5))));

    // line below rect
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(0, 0.5), vec2(1, 1))));

    // line right of rect
    EXPECT_FALSE(l3.intersects(aa_rectangle(vec2(-1, 0), vec2(-0.5, 1))));

    // line left of rect
    EXPECT_FALSE(l3.intersects(aa_rectangle(vec2(0.5, 0), vec2(1, 1))));

    // line touches rect with end point
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(1, -0.5), vec2(2, 0.5))));

    // line touches rect nearly with end point
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(1.001, -0.5), vec2(2, 0.5))));

    // line rouches rect at corner
    EXPECT_FALSE(l1.intersects(aa_rectangle(vec2(2, 0), vec2(4, 2))));
}

}

#pragma clang diagnostic pop
