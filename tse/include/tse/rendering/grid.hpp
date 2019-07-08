#ifndef TSE_INCLUDE_TSE_RENDERING_GRID_HPP
#define TSE_INCLUDE_TSE_RENDERING_GRID_HPP

#include <vector>

#include <tsl/grid.hpp>

#include "tse/gl_buffer.hpp"
#include "tse/rendering/picking_map.hpp"

using std::vector;

namespace tsl {

/**
 * @return pair with <vertex buffer, index buffer>
 */
gl_buffer grid_to_render_buffer(const regular_grid& grid, picking_map& picking_map);
gl_buffer add_grid_to_render_buffer(const regular_grid& grid, gl_buffer& buffer, picking_map& picking_map);

gl_multi_buffer get_multi_render_buffer(const vector<regular_grid>& grids, picking_map& picking_map);

}

#endif //TSE_INCLUDE_TSE_RENDERING_GRID_HPP
