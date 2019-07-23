#ifndef TSL_PLANE_HPP
#define TSL_PLANE_HPP

#include "tsl/geometry/vector.hpp"

namespace tsl {

/**
 * @brief Represents a 3d plane by a support vector and a normal.
 */
struct plane {
    vec3 support_vector;
    vec3 normal;

    /**
     * @brief Creates a 3d plane from the given support vector and the given normal.
     * @param support_vector
     * @param normal
     */
    plane(const vec3& support_vector, const vec3& normal) : support_vector(support_vector), normal(normal) {}
};

}

#endif //TSL_PLANE_HPP
