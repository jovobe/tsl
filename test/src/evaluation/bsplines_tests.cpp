#include <gtest/gtest.h>

#include "tsl/evaluation/bsplines.hpp"

using namespace tsl;

namespace tsl_tests {

TEST(BasisFunsTest, Derivs) {

    // N_2,2(5/2) = 1/8 (NURBS Book page 71)
    // N'_2,2(5/2) = -(1/2) (NURBS Book page 91)
    auto n22 = get_bspline_with_der<2>(2.5, {0,1,2,3,4});
    EXPECT_EQ(1.0/8.0, n22.x);
    EXPECT_EQ(-1.0/2.0, n22.y);

    // N_3,2(5/2) = 6/8 (NURBS Book page 71)
    // N'_3,2(5/2) = 0 (NURBS Book page 91)
    auto n32 = get_bspline_with_der<2>(2.5, {1,2,3,4,4});
    EXPECT_EQ(6.0/8.0, n32.x);
    EXPECT_EQ(0, n32.y);

    // N_4,2(5/2) = 1/8 (NURBS Book page 71)
    // N'_4,2(5/2) = 1/2 (NURBS Book page 91)
    auto n42 = get_bspline_with_der<2>(2.5, {2,3,4,4,5});
    EXPECT_EQ(1.0/8.0, n42.x);
    EXPECT_EQ(1.0/2.0, n42.y);
}

}
