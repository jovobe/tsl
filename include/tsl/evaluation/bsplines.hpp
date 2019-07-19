#ifndef TSL_BSPLINES_HPP
#define TSL_BSPLINES_HPP

#include <utility>
#include <vector>

#include "tsl/geometry/vector.hpp"

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
 * @tparam degree The degree of the B-Spline function.
 * @return A two component vector with x containing the value of the B-Spline function and y containing the value of
 *         the first derivate.
 */
template<uint32_t degree>
vec2 get_bspline_with_der(double u, const vector<double>& knot_vector);

}

#include "bsplines.tcc"

#endif //TSL_BSPLINES_HPP
