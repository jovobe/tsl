#ifndef TSL_GENERATOR_HPP
#define TSL_GENERATOR_HPP

#include "tsl/geometry/tmesh/tmesh.hpp"

namespace tsl {

/**
 * @brief Generates a tmesh in form of a cube with the given edge length and size.
 */
tmesh tmesh_cube(uint32_t size, double edge_length = 1.0);

}

#endif //TSL_GENERATOR_HPP
