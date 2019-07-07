#ifndef TSL_REDUCTION_HPP
#define TSL_REDUCTION_HPP

#include "tsl/geometry/tmesh/tmesh.hpp"

namespace tsl {

// TODO: checks are for the mesh only - but we need to check tmesh
//       integrity as well, if it is still valid after edge removal (new extraordinary vertices could be created!)
size_t remove_edges(tmesh& mesh, double percent);

}

#endif //TSL_REDUCTION_HPP
