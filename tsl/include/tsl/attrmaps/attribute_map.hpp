#ifndef TSL_ATTRIBUTE_MAP_HPP
#define TSL_ATTRIBUTE_MAP_HPP

#include <memory>
#include <optional>
#include <functional>

#include "../util/base_handle.hpp"
#include "../geometry/tmesh/handles.hpp"

using std::optional;
using std::reference_wrapper;

namespace tsl {

// Forward declarations
template<typename> class attribute_map_iterator_ptr;

/**
 * @brief Interface for attribute maps.
 *
 * Attribute maps are associative containers which map from a handle to a
 * value. A simple and obvious implementation of this interface is a hash map.
 *
 * Attribute maps are used a lot in this library and are widely useful. A good
 * example is an algorithm that needs to visit every face by traversing a mesh,
 * but has to make sure to visit every face only once. In that algorithm, the
 * best idea is to use an attribute map which maps from face to bool. This
 * means that we associate a boolean value with each face. This boolean value
 * can be used to store whether or not we already visited that face. Such a
 * map would have the form `attribute_map<face_handle, bool>`.
 *
 * Attribute maps are also used to store non-temporary data, like face-normals,
 * vertex-colors, and much more. It's pretty simple, really: if you want to
 * associate a value of type `T` with a, say, vertex, simply create an
 * `attribute_map<vertex_handle, T>`.
 *
 * There are different implementations of this interface. The most important
 * ones have a type alias in `attr_maps.hpp`. Please read the documentation in
 * that file to learn more about different implementations.
 *
 * @tparam handle_t Key type of this map. Has to inherit from `base_handle`!
 * @tparam value_t The type to map to.
 */
template<typename handle_t, typename value_t>
class attribute_map
{
    static_assert(
        std::is_base_of<base_handle<index>, handle_t>::value,
        "handle_t must inherit from base_handle!"
    );

public:
    /// The type of the handle used as key in this map
    using handle_type = handle_t;

    /// The type of the value stored in this map
    using value_type = value_t;

    /**
     * @brief Returns true iff the map contains a value associated with the
     *        given key.
     */
    virtual bool contains_key(handle_t key) const = 0;

    /**
     * @brief Inserts the given value at the given key position.
     *
     * @return If there was a value associated with the given key before
     *         inserting the new value, the old value is returned. None
     *         otherwise.
     */
    virtual optional<value_t> insert(handle_t key, const value_t& value) = 0;

    /**
     * @brief Removes the value associated with the given key.
     *
     * @return If there was a value associated with the key, it is returned.
     *         None otherwise.
     */
    virtual optional<value_t> erase(handle_t key) = 0;

    /**
     * @brief Removes all values from the map.
     */
    virtual void clear() = 0;

    /**
     * @brief Returns the value associated with the given key or None
     *        if there is no associated value.
     *
     * Note: this method can not be used to insert a new value. It only allows
     * reading and modifying an already inserted value.
     */
    virtual optional<reference_wrapper<value_t>> get(handle_t key) = 0;

    /**
     * @brief Returns the value associated with the given key or None
     *        if there is no associated value.
     *
     * Note: this method can not be used to insert a new value. It only allows
     * reading an already inserted value.
     */
    virtual optional<reference_wrapper<const value_t>> get(handle_t key) const = 0;

    /**
     * @brief Returns the number of values in this map.
     */
    virtual size_t num_values() const = 0;

    /**
     * @brief Returns an iterator over all keys of this map. The order of
     *        iteration is unspecified.
     *
     * You can simply iterate over all keys of this map with a range-based
     * for-loop:
     *
     * \code{.cpp}
     *     for (auto handle: attribute_map) { ... }
     * \endcode
     */
    virtual attribute_map_iterator_ptr<handle_t> begin() const = 0;

    /**
     * @brief Returns an iterator to the end of all keys.
     */
    virtual attribute_map_iterator_ptr<handle_t> end() const = 0;

    /**
     * @brief Returns the value associated with the given key or panics
     *        if there is no associated value.
     *
     * Note: since this method panics, if there is no associated value, it
     * cannot be used to insert new values. Use `insert()` if you want to
     * insert new values.
     */
    value_t& operator[](handle_t key);

    /**
     * @brief Returns the value associated with the given key or panics
     *        if there is no associated value.
     *
     * Note: since this method panics, if there is no associated value, it
     * cannot be used to insert new values. Use `insert()` if you want to
     * insert new values.
     */
    const value_t& operator[](handle_t key) const;

    virtual ~attribute_map() = 0;
};

/**
 * @brief Iterator over keys of an attribute map.
 *
 * This is an interface that has to be implemented by the concrete iterators
 * for the implementors of `attribute_map`.
 */
template<typename handle_t>
class attribute_map_iterator
{
    static_assert(
        std::is_base_of<base_handle<index>, handle_t>::value,
        "handle_t must inherit from base_handle!"
    );

public:
    /// Advances the iterator once. Using the dereference operator afterwards
    /// will yield the next handle.
    virtual attribute_map_iterator& operator++() = 0;
    virtual bool operator==(const attribute_map_iterator& other) const = 0;
    virtual bool operator!=(const attribute_map_iterator& other) const = 0;

    virtual ~attribute_map_iterator() = 0;

    /// Returns the current handle.
    virtual handle_t operator*() const = 0;
    virtual std::unique_ptr<attribute_map_iterator> clone() const = 0;
};

/**
 * @brief Simple convinience wrapper for unique_ptr<attribute_map_iterator>
 *
 * The unique_ptr is needed to return an abstract class. This `ptr` class
 * enables the user to easily use this smart pointer as iterator.
 */
template<typename handle_t>
class attribute_map_iterator_ptr
{
    static_assert(
        std::is_base_of<base_handle<index>, handle_t>::value,
        "handle_t must inherit from base_handle!"
    );

public:
    explicit attribute_map_iterator_ptr(std::unique_ptr<attribute_map_iterator<handle_t>> iter)
        : iter(std::move(iter)) {}
    attribute_map_iterator_ptr(const attribute_map_iterator_ptr& iterator_ptr);
    attribute_map_iterator_ptr(attribute_map_iterator_ptr&& iterator_ptr) noexcept;

    ~attribute_map_iterator_ptr();

    attribute_map_iterator_ptr& operator=(const attribute_map_iterator_ptr& iterator_ptr);
    attribute_map_iterator_ptr& operator=(attribute_map_iterator_ptr&& iterator_ptr) noexcept;

    attribute_map_iterator_ptr& operator++();
    bool operator==(const attribute_map_iterator_ptr& other) const;
    bool operator!=(const attribute_map_iterator_ptr& other) const;
    handle_t operator*() const;

private:
    std::unique_ptr<attribute_map_iterator<handle_t>> iter;
};


}

#include "attribute_map.tcc"

#endif //TSL_ATTRIBUTE_MAP_HPP
