#ifndef INCLUDE_TSL_ROW_HPP
#define INCLUDE_TSL_ROW_HPP

#include <vector>

#include <tsl/geometry/vector.hpp>
#include <tsl/geometry/tmesh/handles.hpp>

using std::vector;

namespace tsl {

struct row {
    row(const edge_handle& handle, const vertex_handle& start): handle(handle), start(start) {}

    vector<vec3> points;
    vector<vec3> normals;
    edge_handle handle;
    vertex_handle start;
};

}

#endif //INCLUDE_TSL_ROW_HPP
