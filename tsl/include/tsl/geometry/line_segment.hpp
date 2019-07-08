#ifndef TSL_LINE_SEGMENT_HPP
#define TSL_LINE_SEGMENT_HPP

#include "rectangle.hpp"
#include "vector.hpp"

namespace tsl {

struct line_segment {
    vec2 p1;
    vec2 p2;

    line_segment(const vec2& p1, const vec2& p2) : p1(p1), p2(p2) {}

    /**
     * @brief Returns true, if the line intersects the interior of the given rect.
     *
     * If the line touches the rect without intersecting its interior, false will be returned!
     */
    bool intersects(const aa_rectangle& rectangle) const;

private:
    enum class side {
        on,
        left,
        right
    };
    side test(vec2 p) const;
};

}

#endif //TSL_LINE_SEGMENT_HPP
