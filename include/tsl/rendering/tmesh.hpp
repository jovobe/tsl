#ifndef TSL_RENDERING_TMESH_HPP
#define TSL_RENDERING_TMESH_HPP

#include <vector>
#include <functional>

#include <tsl/grid.hpp>

using std::vector;
using std::reference_wrapper;

namespace tsl
{

vector<uint8_t> get_picked_faces_buffer(const vector<regular_grid>& faces, const vector<reference_wrapper<const picking_element>>& picked);

}

#endif //TSL_RENDERING_TMESH_HPP
