#ifndef TSL_REDUCTION_HPP
#define TSL_REDUCTION_HPP

#include "tsl/geometry/tmesh/tmesh.hpp"

namespace tsl {

/**
 * @brief Tries to remove the given percentage of egdges from the given tmesh.
 */
size_t remove_edges(tmesh& mesh, double percent);

}

#endif //TSL_REDUCTION_HPP
