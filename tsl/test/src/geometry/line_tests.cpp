#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <glm/glm.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tsl/geometry/vector.hpp>
#include <tsl/geometry/line.hpp>
#include <tsl/geometry/plane.hpp>

using glm::normalize;

using namespace tsl;

namespace tsl_tests {

class LineTest : public ::testing::Test {
protected:
    LineTest() : l1(vec3(0, 0, 0), vec3(1, 0, 0)), p1(vec3(0, 0, 1), normalize(vec3(1, 0, 1))) {}

    // The x axis as a line
    line l1;

    // A plane x, y plane, with angle of 45° rotated around y at point (0, 0, 1)
    plane p1;
};

TEST_F(LineTest, IntersectPlane) {
    auto intersection = l1.intersect(p1);
    EXPECT_TRUE(intersection);
    EXPECT_EQ((*intersection), vec3(1, 0, 0));
}

}

#pragma clang diagnostic pop
