#ifndef TSL_PLANE_HPP
#define TSL_PLANE_HPP

#include <tsl/geometry/vector.hpp>

namespace tsl {

struct plane {
    vec3 support_vector;
    vec3 normal;

    plane(const vec3& support_vector, const vec3& normal) : support_vector(support_vector), normal(normal) {}
};

}

#endif //TSL_PLANE_HPP
