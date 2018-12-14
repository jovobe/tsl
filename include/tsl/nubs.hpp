#ifndef TSL_NUBS_HPP
#define TSL_NUBS_HPP

#include <tsl/grid.hpp>

#include <vector>

#include <glm/glm.hpp>

using std::vector;

using glm::vec3;

namespace tsl {

/**
 * c: number of control points
 * p: degree
 * m: number of knots
 * n: c + 1
 * m=n+p+1
 */
struct nubs {

    // degree of u-curve
    uint32_t p;

    // degree of v-curve
    uint32_t q;

    // knot vector for u-curve
    vector<float> U;

    // knot vector for v-curve
    vector<float> V;

    // matrix of control points
    regular_grid P;

    nubs() : p(0), q(0), U(), V(), P() {};
};

nubs get_example_data_1();

}

#endif //TSL_NUBS_HPP
