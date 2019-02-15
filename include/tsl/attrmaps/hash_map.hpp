#ifndef TSL_HASH_MAP_HPP
#define TSL_HASH_MAP_HPP

#include <unordered_map>
#include <optional>
#include <functional>

#include <tsl/attrmaps/attribute_map.hpp>

using std::unordered_map;
using std::optional;
using std::reference_wrapper;

namespace tsl {

template<typename handle_t, typename value_t>
class hash_map : public attribute_map<handle_t, value_t>
{
public:
    /**
     * @brief Creates an empty map without default element set.
     */
    hash_map() = default;

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
    explicit hash_map(const value_t& default_value);

    /**
     * @brief Creates a map with a given default value and calls reserve.
     *
     * This works exactly as the `hash_map(const value&)` constructor, but
     * also calls `reserve(count_elements)` immediately afterwards.
     */
    hash_map(size_t count_elements, const value_t& default_value);

    ~hash_map() override;

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
     * @brief Allocates space for at least `new_cap` more elements.
     */
    void reserve(size_t new_cap);

private:
    unordered_map<handle_t, value_t> map;
    optional<value_t> default_value;
};

}

#include <tsl/attrmaps/hash_map.tcc>

#endif //TSL_HASH_MAP_HPP
