#ifndef TSL_LINE_SEGMENT_HPP
#define TSL_LINE_SEGMENT_HPP

#include "tsl/geometry/rectangle.hpp"
#include "tsl/geometry/vector.hpp"

namespace tsl {

/**
 * @brief Represents a 2d line segment by to points.
 */
struct line_segment {
    /// Start point
    vec2 p1;
    /// End point.
    vec2 p2;

    /**
     * @brief Creates a line segment, which lies between the given points.
     * @param p1 Start point.
     * @param p2 End point.
     */
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
