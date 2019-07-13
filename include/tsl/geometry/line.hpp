#ifndef TSL_LINE_HPP
#define TSL_LINE_HPP

#include <optional>

#include "tsl/geometry/vector.hpp"
#include "tsl/geometry/plane.hpp"

using std::optional;

namespace tsl {

/**
 * @brief Represents a 3d line.
 */
struct line {
    /// Support vector of the line.
    vec3 support_vector;
    /// Normal (or direction) vector of the line.
    vec3 normal;

    /**
     * @brief Creates a 3d line with the given support vector and the given normal.
     * @param support_vector
     * @param normal
     */
    line(const vec3& support_vector, const vec3& normal) : support_vector(support_vector), normal(normal) {}

    /**
     * @brief Calculates the intersection point of this line with the given plane. If they are parallel, none will be
     * returned.
     */
    optional<vec3> intersect(const plane& plane) const;
};

}

#endif //TSL_LINE_HPP
