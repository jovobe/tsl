#ifndef TSL_LINE_HPP
#define TSL_LINE_HPP

#include "vector.hpp"
#include "plane.hpp"

#include <optional>

using std::optional;

namespace tsl {

struct line {
    vec3 support_vector;
    vec3 normal;

    line(const vec3& support_vector, const vec3& normal) : support_vector(support_vector), normal(normal) {}

    optional<vec3> intersect(const plane& plane) const;
};

}

#endif //TSL_LINE_HPP
