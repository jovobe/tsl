#ifndef TSL_VECTOR_MAP_HPP
#define TSL_VECTOR_MAP_HPP

#include <optional>
#include <functional>

#include <tsl/attrmaps/attribute_map.hpp>
#include <tsl/attrmaps/stable_vector.hpp>

using std::optional;
using std::reference_wrapper;

namespace tsl {

/**
 * @brief A map with constant lookup overhead using small-ish integer-keys.
 *
 * It stores the given values in a vector, they key is simply the index within
 * the vector. This means that the space requirement is O(largest_key). See
 * stable_vector for more information.
 */
template<typename handle_t, typename value_t>
class vector_map : public attribute_map<handle_t, value_t>
{
public:
    /**
     * @brief Creates an empty map without default element set.
     */
    vector_map() = default;

    /**
     * @brief Creates a map with a given default value.
     *
     * Whenever you request a value for a key and there isn't a value
     * associated with that key, the default value is returned.  Note that if
     * you set a default value (which you do by calling this constructor), you
     * can't remove it. Neither `erase()` nor `clear()` will do it. Calls to
     * `get()` will always return a non-none value and `operator[]` won't ever
     * panic.
     *
     * One additional important detail: if you call `get()` to obtain a
     * mutable reference, the default value is inserted into the map. This is
     * the only sane way to return a mutably reference.
     */
    explicit vector_map(const value_t& default_value);

    /**
     * @brief Creates a map with a given default value and calls reserve.
     *
     * This works exactly as the `vector_map(const value&)` constructor, but
     * also calls `reserve(count_elements)` immediately afterwards.
     */
    vector_map(size_t count_elements, const value_t& default_value);

    ~vector_map() override;

    // =======================================================================
    // Implemented methods from the interface (check interface for docs)
    // =======================================================================
    bool contains_key(handle_t key) const final;
    optional<value_t> insert(handle_t key, const value_t& value) final;
    optional<value_t> erase(handle_t key) final;
    void clear() final;
    optional<reference_wrapper<value_t>> get(handle_t key) final;
    optional<reference_wrapper<const value_t>> get(handle_t key) const final;
    size_t num_values() const final;

    /**
     * @see stable_vector::reserve(size_t)
     */
    void reserve(size_t new_cap);

private:
    /// The underlying storage
    stable_vector<handle_t, value_t> vec;
    optional<value_t> default_value;
};

}

#include <tsl/attrmaps/vector_map.tcc>

#endif //TSL_VECTOR_MAP_HPP
