#ifndef TSL_ATTR_MAPS_HPP
#define TSL_ATTR_MAPS_HPP

#include "attribute_map.hpp"
#include "hash_map.hpp"
#include "vector_map.hpp"
#include "../geometry/tmesh/handles.hpp"

namespace tsl {

/*
 * This file defines many useful type aliases for implementations of the
 * `attribute_map` interface.
 *
 * Choosing the correct implementation can have a huge impact on performance
 * of your algorithm. There are two main implementations, where each is
 * useful in a specific situation -- it mainly depends on the number of values
 * in the map.
 *
 * - dense_attr_map: if there is a value associated with almost all handles
 * - sparse_attr_map: if the number of values is significantly less than the
 *                  number of handles
 *
 * In some algorithms you will associate a value with *every* handle, e.g. a
 * `vertex_map<bool> visited`. In this situation, it's useful to use the
 * dense_attr_map: it will be faster than the other two implementations. In other
 * situations, however, you won't associate a value with most handles. Here,
 * a dense_attr_map would be a bad idea, because it has a memory requirement of
 * O(biggest_handle_idx). This means, a lot of space would be wasted. Thus,
 * rather use sparse_attr_map in that case. Each lookup will be a bit slower, but
 * we won't waste memory.
 *
 * It's useful to look at HOW these implementations work under the hood to
 * understand the runtime and memory overhead of each:
 *
 * - dense_attr_map: uses an array (vector_map/std::vector)
 * - sparse_attr_map: uses a hash map (hash_map/std::unordered_map)
 *
 *
 * Additionally, there are specific type aliases for the most common uses,
 * like `face_map = attribute_map<face_handle, T>`. You should use those when you
 * can.
 */

// ---------------------------------------------------------------------------
// Generic aliases
template<typename handle_t, typename value_t> using dense_attr_map  = vector_map<handle_t, value_t>;
template<typename handle_t, typename value_t> using sparse_attr_map = hash_map<handle_t, value_t>;

// ---------------------------------------------------------------------------
// Handle-specific aliases
template<typename value_t> using edge_map       = attribute_map<edge_handle, value_t>;
template<typename value_t> using half_edge_map  = attribute_map<half_edge_handle, value_t>;
template<typename value_t> using face_map     = attribute_map<face_handle, value_t>;
template<typename value_t> using vertex_map   = attribute_map<vertex_handle, value_t>;

// ---------------------------------------------------------------------------
// Handle- and implementation-specific aliases
template<typename value_t> using dense_edge_map        = dense_attr_map<edge_handle, value_t>;
template<typename value_t> using dense_half_edge_map        = dense_attr_map<half_edge_handle, value_t>;
template<typename value_t> using dense_face_map        = dense_attr_map<face_handle, value_t>;
template<typename value_t> using dense_vertex_map      = dense_attr_map<vertex_handle, value_t>;

template<typename value_t> using sparse_edge_map       = sparse_attr_map<edge_handle, value_t>;
template<typename value_t> using sparse_half_edge_map       = sparse_attr_map<half_edge_handle, value_t>;
template<typename value_t> using sparse_face_map       = sparse_attr_map<face_handle, value_t>;
template<typename value_t> using sparse_vertex_map     = sparse_attr_map<vertex_handle, value_t>;

}

#endif //TSL_ATTR_MAPS_HPP
