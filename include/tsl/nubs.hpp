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
 * m+1: number of knots
 * n: c - 1
 * m=n+p+1
 *
 * curve x: U, x, n, p
 * curve y: V, y, m, p
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

    // number of control points in x direction
    uint32_t x;

    // number of control points in y direction
    uint32_t y;

    // n: x + 1
    uint32_t n;

    // m: y + 1
    uint32_t m;

    nubs() : p(0), q(0), U(), V(), P(face_handle(0)), x(), y(), n(), m() {};

    regular_grid get_grid(uint32_t resolution) const;
    vec3 get_surface_point(float u, float v) const;
    size_t get_u_span(float u) const;
    size_t get_v_span(float v) const;
    vector<float> get_u_basis_funs(size_t span, float u) const;
    vector<float> get_v_basis_funs(size_t span, float v) const;

    static nubs get_example_data_1();
    static nubs get_example_data_2();
    static nubs get_example_data_3();
    static nubs get_example_data_4();
};

}

#endif //TSL_NUBS_HPP
