#ifndef TSL_RECTANGLE_HPP
#define TSL_RECTANGLE_HPP

#include "vector.hpp"

namespace tsl {

struct aa_rectangle {
    vec2 bottom_left;
    vec2 top_right;

    aa_rectangle(const vec2& p1, const vec2& p2);
};

}

#endif //TSL_RECTANGLE_HPP
