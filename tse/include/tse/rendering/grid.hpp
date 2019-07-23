#ifndef TSE_GRID_HPP
#define TSE_GRID_HPP

#include <cstdint>
#include <vector>

#include <tsl/geometry/vector.hpp>
#include <tsl/grid.hpp>

#include "tse/gl_buffer.hpp"
#include "tse/rendering/picking_map.hpp"

using std::vector;

using tsl::regular_grid;

namespace tse {

/**
 * @brief Converts the given grid into a `gl_buffer`.
 */
gl_buffer get_render_buffer(const regular_grid& grid, picking_map& picking_map);

/**
 * @brief Adds the given grid into the given `gl_buffer`.
 */
gl_buffer add_to_render_buffer(const regular_grid& grid, gl_buffer& buffer, picking_map& picking_map);

/**
 * @brief Converts a vector of `regular_grid` into a `gl_multi_buffer`.
 */
gl_multi_buffer get_multi_render_buffer(const vector<regular_grid>& grids, picking_map& picking_map);

}

#endif //TSE_GRID_HPP
