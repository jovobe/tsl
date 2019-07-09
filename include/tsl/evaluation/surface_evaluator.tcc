#include <algorithm>

#include <glm/glm.hpp>

using std::min;

using glm::cross;
using glm::normalize;

namespace tsl {

template<typename eval_t>
regular_grid surface_evaluator::eval_loop(uint32_t res, face_handle handle, eval_t evaluator, bool skip_edges) const {
    if (skip_edges) {
        res += 2;
    }

    auto local_system_max = get_max_coords(handle);
    double u_coord = local_system_max.x;
    double v_coord = local_system_max.y;
    auto u_max = res + 1u;
    auto v_max = res + 1u;
    double step_u = u_coord / res;
    double step_v = v_coord / res;

    if (skip_edges) {
        u_max -= 2;
        v_max -= 2;
    }

    regular_grid grid(handle);
    grid.points.reserve(static_cast<size_t>(v_max));
    grid.normals.reserve(static_cast<size_t>(v_max));
    grid.num_points_x = u_max;
    grid.num_points_y = v_max;

    double current_u = 0;
    double current_v = 0;
    if (skip_edges) {
        current_v = step_v;
    }

    for (uint32_t v = 0; v < v_max; ++v) {
        current_u = 0;
        if (skip_edges) {
            current_u = step_u;
        }

        vector<vec3> row;
        vector<vec3> normal_row;
        row.reserve(static_cast<size_t>(u_max));
        normal_row.reserve(static_cast<size_t>(u_max));
        for (uint32_t u = 0; u < u_max; ++u) {
            auto[point, du, dv] = evaluator(min(current_u, u_coord), min(current_v, v_coord), handle);
            row.push_back(point);
            normal_row.push_back(normalize(cross(du, dv)));
            current_u += step_u;
        }
        grid.points.push_back(row);
        grid.normals.push_back(normal_row);
        current_v += step_v;
    }
    return grid;
}

}
