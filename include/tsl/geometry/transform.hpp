#ifndef TSL_TRANSFORM_HPP
#define TSL_TRANSFORM_HPP

#include "vector.hpp"

namespace tsl {

/**
 * @brief Represents a transformation of a 2d vector between two coordinate systems.
 */
struct transform {
    /// Scaling factor of this transformation.
    double f;
    /// Rotation by a multiple of 90°.
    uint8_t r;
    /// Translation part of this transformation.
    vec2 t;

    /**
     * @brief Creates a transform.
     * @param f Scaling factor of this transformation.
     * @param r Rotation by a multiple of 90°.
     * @param t Translation part of this transformation.
     */
    transform(double f, uint8_t r, vec2 t) : f(f), r(r), t(t) {}

    /**
     * @brief Apply this transform (t1) on the given transform (t2) and return the resulting transform.
     *
     * In other words: this is t1 and the given transform is t2 the result is t1⚬t2 or t1(t2).
     */
    transform apply(const transform& trans) const;

    /**
     * @brief Transforms the given vector and returns the result.
     */
    vec2 apply(const vec2& vec) const;
};

/**
 * @brief Rotates the given 2d vector `times` times by a multiple of 90°.
 */
vec2 rotate(uint8_t times, const vec2& vec);

}

#endif //TSL_TRANSFORM_HPP
