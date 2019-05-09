#ifndef TSL_OBJ_HPP
#define TSL_OBJ_HPP

#include <tsl/geometry/half_edge_mesh.hpp>

#include <string>

using std::string;

namespace tsl {

half_edge_mesh read_obj_into_hem(const string& file_path);

}

#endif //TSL_OBJ_HPP
