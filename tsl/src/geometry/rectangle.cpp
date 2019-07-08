#include <algorithm>

#include <tsl/geometry/rectangle.hpp>

using std::max;
using std::min;

namespace tsl {

aa_rectangle::aa_rectangle(const vec2& p1, const vec2& p2) {
    top_right = vec2(max(p1.x, p2.x), max(p1.y, p2.y));
    bottom_left = vec2(min(p1.x, p2.x), min(p1.y, p2.y));
}

}
