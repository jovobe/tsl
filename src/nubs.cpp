#include <tsl/nubs.hpp>

#include <glm/glm.hpp>

using glm::vec3;

namespace tsl {

nubs get_example_data_1() {

    nubs out;
    out.p = 2;
    out.q = 2;

    // 8
    out.U = {0, 0, 0, 2.0f/5, 3.0f/5, 1, 1, 1};
    // 9
    out.V = {0, 0, 0, 1.0f/5, 1.0f/2, 4.0f/5, 1, 1, 1};

    // grid size: 5 x 6
    out.P.points = {
        {vec3(0, 0, 0), vec3(1, 0, 0), vec3(2, 0, 0), vec3(3, 0, 0), vec3(4, 0, 0), vec3(5, 0, 0)},
        {vec3(0, 1, 0), vec3(1, 1, 0), vec3(2, 1, 0), vec3(3, 1, 0), vec3(4, 1, 0), vec3(5, 1, 0)},
        {vec3(0, 2, 0), vec3(1, 2, 0), vec3(2, 2, 0), vec3(3, 2, 0), vec3(4, 2, 0), vec3(5, 2, 0)},
        {vec3(0, 3, 0), vec3(1, 3, 0), vec3(2, 3, 0), vec3(3, 3, 1), vec3(4, 3, 1.5), vec3(5, 3, 1.5)},
        {vec3(0, 4, 0), vec3(1, 4, 0), vec3(2, 4, 0), vec3(3, 4, 1), vec3(4, 4, 1.5), vec3(5, 4, 1.5)},
//        {vec3(0, 5, 0), vec3(1, 5, 0), vec3(2, 5, 0), vec3(3, 5, 0), vec3(4, 5, 0), vec3(5, 5, 0)},
    };
//    out.P.points = {
//        {vec3(0, 0, 0), vec3(1, 0, 0), vec3(2, 0, 0)},
//        {vec3(0, 1, 0), vec3(1, 1, 0), vec3(2, 1, 0)},
//        {vec3(0, 2, 0), vec3(1, 2, 0), vec3(2, 2, 0)},
//    };

    return out;
}

}
