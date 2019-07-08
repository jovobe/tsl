#ifndef TSL_TRANSFORM_HPP
#define TSL_TRANSFORM_HPP

#include "vector.hpp"

namespace tsl {

struct transform {
    double f;
    uint8_t r;
    vec2 t;

    transform(double f, uint8_t r, vec2 t) : f(f), r(r), t(t) {}

    /**
     * @brief Apply this transform (t1) on the given transform (t2) and return the resulting transform.
     *
     * In other words: this is t1 and the given transform is t2 the result is t1⚬t2 or t1(t2).
     */
    transform apply(const transform& trans) const;
    vec2 apply(const vec2& vec) const;
};

vec2 rotate(uint8_t times, const vec2& vec);

}

#endif //TSL_TRANSFORM_HPP
