#include "tsl/geometry/transform.hpp"

namespace tsl {

transform transform::apply(const transform& trans) const {
    auto scale = f * trans.f;
    auto rotate = static_cast<uint8_t>((r + trans.r) % 4);
    auto translate = apply(trans.t);
    return transform(scale, rotate, translate);
}

vec2 transform::apply(const vec2& vec) const {
    return f * rotate(r, vec) + t;
}

vec2 rotate(uint8_t times, const vec2& vec) {
    vec2 out(vec);
    switch (times % 4) {
        case 1: {
            auto swp = out.y;
            out.y = out.x;
            out.x = swp * -1;
            break;
        }
        case 2:
            out *= -1;
            break;
        case 3: {
            auto swp = out.y;
            out.y = out.x * -1;
            out.x = swp;
            break;
        }
        default:
            // Catches case 0, where the input stays the same
            break;
    }
    return out;
}

}
