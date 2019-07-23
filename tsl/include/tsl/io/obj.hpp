#ifndef TSL_OBJ_HPP
#define TSL_OBJ_HPP

#include <string>

#include "tsl/geometry/tmesh/tmesh.hpp"

using std::string;

namespace tsl {

/**
 * @brief Imports the quadmesh from the given file path into a tmesh and returns it.
 */
tmesh read_obj_into_tmesh(const string& file_path);

}

#endif //TSL_OBJ_HPP
