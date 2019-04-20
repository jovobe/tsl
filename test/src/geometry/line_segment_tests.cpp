#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tsl/geometry/line_segment.hpp>
#include <tsl/geometry/vector.hpp>

using namespace tsl;

namespace tsl_tests {

class LineSegmentsTest : public ::testing::Test {
protected:
    LineSegmentsTest() : l1(vec2(0, 0), vec2(3, 3)), l2(vec2(0, 0), vec2(1, 0)), l3(vec2(0, 0), vec2(0, 1)) {}

    // Diagonal line_segment
    line_segment l1;

    // Line x-axis alined
    line_segment l2;

    // Line y-axis alined
    line_segment l3;
};

TEST_F(LineSegmentsTest, Intersects) {
    // line_segment in rect
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(0, 0), vec2(1, 1))));
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(-0.5, -0.5), vec2(5, 5))));

    // line_segment through rect
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(0.5, 0.5), vec2(1, 1))));

    // line_segment into recht
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(0.5, 0.5), vec2(5, 5))));

    // line_segment out from rect
    EXPECT_TRUE(l1.intersects(aa_rectangle(vec2(-0.5, -0.5), vec2(1, 1))));

    // line_segment on rect edge
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(0, 0), vec2(1, 1))));

    // line_segment above rect
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(-1, -1), vec2(-1, -0.5))));

    // line_segment below rect
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(0, 0.5), vec2(1, 1))));

    // line_segment right of rect
    EXPECT_FALSE(l3.intersects(aa_rectangle(vec2(-1, 0), vec2(-0.5, 1))));

    // line_segment left of rect
    EXPECT_FALSE(l3.intersects(aa_rectangle(vec2(0.5, 0), vec2(1, 1))));

    // line_segment touches rect with end point
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(1, -0.5), vec2(2, 0.5))));

    // line_segment touches rect nearly with end point
    EXPECT_FALSE(l2.intersects(aa_rectangle(vec2(1.001, -0.5), vec2(2, 0.5))));

    // line_segment rouches rect at corner
    EXPECT_FALSE(l1.intersects(aa_rectangle(vec2(2, 0), vec2(4, 2))));
}

}

#pragma clang diagnostic pop
