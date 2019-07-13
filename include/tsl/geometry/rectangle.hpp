#ifndef TSL_RECTANGLE_HPP
#define TSL_RECTANGLE_HPP

#include "tsl/geometry/vector.hpp"

namespace tsl {

/**
 * @brief Represents a 2d axis aligned rectangle.
 */
struct aa_rectangle {
    vec2 bottom_left;
    vec2 top_right;

    /**
     * @brief Creates a 2d axis aligned rectangle from the given points.
     * @param p1 Bottom left point.
     * @param p2 Top right point.
     */
    aa_rectangle(const vec2& p1, const vec2& p2);
};

}

#endif //TSL_RECTANGLE_HPP
