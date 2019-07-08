#ifndef TSL_BSPLINES_HPP
#define TSL_BSPLINES_HPP

#include <utility>
#include <vector>

#include "../geometry/vector.hpp"

using std::pair;
using std::vector;

namespace tsl {

/**
 * @brief Calculates the value of the B-Spline function and the first derivate for the given u, on the given knot
 *        vector for the given degree.
 *
 * The knot vector length has to match degree + 2.
 *
 * @param u The input for the B-Spline function.
 * @param knot_vector The knot vector on which the B-Spline function will be calculated.
 * @param degree The degree of the B-Spline function.
 * @return A two component vector with x containing the value of the B-Spline function and y containing the value of
 *         the first derivate.
 */
vec2 get_bspline_with_der(double u, const vector<double>& knot_vector, uint32_t degree = 3);

}

#endif //TSL_BSPLINES_HPP
