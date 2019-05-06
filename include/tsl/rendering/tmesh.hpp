#ifndef TSL_RENDERING_TMESH_HPP
#define TSL_RENDERING_TMESH_HPP

#include <vector>
#include <set>
#include <functional>

#include <tsl/grid.hpp>

using std::vector;
using std::reference_wrapper;
using std::set;

namespace tsl
{

vector<uint8_t> get_picked_faces_buffer(const vector<regular_grid>& faces, const set<picking_element>& picked);

}

#endif //TSL_RENDERING_TMESH_HPP
