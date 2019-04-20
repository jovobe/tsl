#include <limits>
#include <optional>

#include <tsl/geometry/line_segment.hpp>

using std::numeric_limits;
using std::optional;

namespace tsl {

bool line_segment::intersects(const aa_rectangle& rectangle) const {
    auto corners = {
        rectangle.bottom_left,
        vec2(rectangle.bottom_left.x, rectangle.top_right.y),
        vec2(rectangle.top_right.x, rectangle.bottom_left.y),
        rectangle.top_right
    };

    bool need_further_check = false;
    optional<side> last;
    for (auto&& c: corners) {
        auto t = test(c);
        if (last) {
            if ((t != side::on && *last != side::on) && *last != t) {
                need_further_check = true;
                break;
            }
        }
        last = t;
    }

    if (!need_further_check) {
        return false;
    }

    return !(
        // line_segment is right of rect
        (p1.x >= rectangle.top_right.x && p2.x >= rectangle.top_right.x)
        ||
        // line_segment is left of rect
        (p1.x <= rectangle.bottom_left.x && p2.x <= rectangle.bottom_left.x)
        ||
        // line_segment is above of rect
        (p1.y >= rectangle.top_right.y && p2.y >= rectangle.top_right.y)
        ||
        // line_segment is below of rect
        (p1.y <= rectangle.bottom_left.y && p2.y <= rectangle.bottom_left.y)
     );
}

line_segment::side line_segment::test(vec2 p) const {
    auto test = (p2.y - p1.y) * p.x + (p1.x - p2.x) * p.y + (p2.x * p1.y - p1.x * p2.y);
    if (test < numeric_limits<double>::epsilon() && test > -numeric_limits<double>::epsilon()) {
        return side::on;
    } else if (test > 0) {
        return side::left;
    } else {
        return side::right;
    }
}

}
