#include <tsl/geometry/line.hpp>

#include <glm/glm.hpp>

#include <optional>

using std::optional;
using std::nullopt;

using glm::dot;

namespace tsl {

optional<vec3> line::intersect(const plane& plane) const {
    // The line is parallel to the plane
    if (dot(plane.normal, normal) == 0) {
        return nullopt;
    }

    auto w = support_vector - plane.support_vector;
    auto s_i = -dot(plane.normal, w) / dot(plane.normal, normal);

    return support_vector + (s_i * normal);
}

}
