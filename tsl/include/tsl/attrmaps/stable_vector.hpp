#ifndef TSL_STABLE_VECTOR_HPP
#define TSL_STABLE_VECTOR_HPP

#include <optional>
#include <vector>
#include <functional>

#include "../util/base_handle.hpp"
#include "../geometry/tmesh/handles.hpp"

using std::optional;
using std::vector;
using std::reference_wrapper;

namespace tsl {

/**
 * @brief Iterator over handles in this vector, which skips deleted elements
 *
 * Important: This is NOT a fail fast iterator. If the vector is changed while
 * using an instance of this iterator the behavior is undefined!
 */
template<typename handle_t, typename elem_t>
class stable_vector_iterator
{
private:
    /// Reference to the deleted marker array this iterator belongs to
    const vector<optional<elem_t>>* elements;

    /// Current position in the vector
    size_t pos;
public:
    explicit stable_vector_iterator(const vector<optional<elem_t>>* deleted, bool start_at_end = false);

    stable_vector_iterator& operator=(const stable_vector_iterator& other);
    bool operator==(const stable_vector_iterator& other) const;
    bool operator!=(const stable_vector_iterator& other) const;

    stable_vector_iterator& operator++();

    bool is_at_end() const;

    handle_t operator*() const;
};

/**
 * @brief A vector which guarantees stable indices and features O(1) deletion.
 *
 * This is basically a wrapper for the std::vector, which marks an element as
 * deleted but does not actually delete it. This means that indices are never
 * invalidated. When inserting an element, you get its index (its so called
 * "handle") back. This handle can later be used to access the element. This
 * remains true regardless of other insertions and deletions happening in
 * between.
 *
 * USE WITH CAUTION: This NEVER frees memory of deleted values (except on its
 * own destruction and can get very large if used incorrectly! If deletions in
 * your use-case are far more numerous than insertions, this data structure is
 * probably not fitting your needs. The memory requirement of this class is
 * O(n_p) where n_p is the number of `push()` calls.
 *
 * @tparam handle_t This handle type contains the actual index. It has to be
 *                 derived from `base_handle`!
 * @tparam elem_t Type of elements in the vector.
 */
template<typename handle_t, typename elem_t>
class stable_vector {
    static_assert(
        std::is_base_of<base_handle<index>, handle_t>::value,
        "handle_t must inherit from base_handle!"
    );

public:
    using element_type = elem_t;
    using handle_type = handle_t;

    /**
     * @brief Creates an empty stable_vector.
     */
    stable_vector() : used_count(0) {};

    /**
     * @brief Creates a stable_vector with `count_elements` many copies of
     *        `default_value`.
     *
     * The elements are stored contiguously in the vectors, thus the valid
     * indices of these elements are 0 to `count_elements` - 1.
     */
    stable_vector(size_t count_elements, const element_type& default_value);

    /**
     * @brief Adds a copy of the given element to the vector.
     *
     * @return The handle referring to the inserted element.
     */
    handle_type push(const element_type& elem);

    /**
     * @brief Adds the given element by moving from it.
     *
     * @return The handle referring to the inserted element.
     */
    handle_type push(element_type&& elem);

    /**
     * @brief Increases the size of the vector to the length of `up_to`.
     *
     * This means that the next call to `push()` after calling `resize(up_to)`
     * will return exactly the `up_to` handle. All elements that are inserted
     * by this method are marked as deleted and thus aren't initialized. They
     * can be set later with `set()`.
     *
     * If `up_to` is already a valid handle, this method will panic!
     */
    void increase_size(handle_type up_to);

    /**
     * @brief Increases the size of the vector to the length of `up_to` by
     *        inserting copies of `elem`.
     *
     * This means that the next call to `push()` after calling `resize(up_to)`
     * will return exactly the `up_to` handle.
     *
     * If `up_to` is already a valid handle, this method will panic!
     */
    void increase_size(handle_type up_to, const element_type& elem);

    /**
     * @brief The handle which would be returned by calling `push` now.
     */
    handle_type next_handle() const;

    /**
     * @brief Mark the element behind the given handle as deleted.
     *
     * While the element is deleted, the handle stays valid. This means that
     * trying to obtain the element with this handle later, will always result
     * in `none` (if `get()` was used). Additionally, the handle can also be
     * used with the `set()` method.
     */
    void erase(handle_type handle);

    /**
     * @brief Removes all elements from the vector.
     */
    void clear();

    /**
     * @brief Returns the element referred to by `handle`.
     *
     * Returns `none` if the element was deleted or if the handle is out of
     * bounds.
     */
    optional<reference_wrapper<element_type>> get(handle_type handle);

    /**
     * @brief Returns the element referred to by `handle`.
     *
     * Returns `none` if the element was deleted or if the handle is out of
     * bounds.
     */
    optional<reference_wrapper<const element_type>> get(handle_type handle) const;

    /**
     * @brief Set a value for the existing `handle`.
     *
     * In this method, the `handle` has to be valid: it has to be obtained by
     * a prior `push()` call. If you want to insert a new element, use `push()`
     * instead of this `set()` method!
     */
    void set(handle_type handle, const element_type& elem);

    /**
     * @brief Set a value for the existing `handle` by moving from `elem`.
     *
     * In this method, the `handle` has to be valid: it has to be obtained by
     * a prior `push()` call. If you want to insert a new element, use `push()`
     * instead of this `set()` method!
     */
    void set(handle_type handle, element_type&& elem);

    /**
     * @brief Returns the element referred to by `handle`.
     *
     * If `handle` is out of bounds or the element was deleted, this method
     * will throw an exception in debug mode and has UB in release mode. Use
     * `get()` instead to gracefully handle the absence of an element.
     */
    element_type& operator[](handle_type handle);

    /**
     * @brief Returns the element referred to by `handle`.
     *
     * If `handle` is out of bounds or the element was deleted, this method
     * will throw an exception in debug mode and has UB in release mode. Use
     * `get()` instead to gracefully handle the absence of an element.
     */
    const element_type& operator[](handle_type handle) const;

    /**
     * @brief Absolute size of the vector (including deleted elements).
     */
    size_t size() const;

    /**
     * @brief Number of non-deleted elements.
     */
    size_t num_used() const;

    /**
     * @brief Returns an iterator to the first element of this vector.
     *
     * This iterator auto skips deleted elements and returns handles to the
     * valid elements.
     */
    stable_vector_iterator<handle_type, element_type> begin() const;

    /**
     * @brief Returns an iterator to the element after the last element of
     *        this vector.
     */
    stable_vector_iterator<handle_type, element_type> end() const;

    /**
     * @brief Increase the capacity of the vector to a value that's greater or
     *        equal to new_cap.
     *
     * If new_cap is greater than the current capacity, new storage is
     * allocated, otherwise the method does nothing.
     *
     * @param new_cap new capacity of the vector
     */
    void reserve(size_t new_cap);

private:
    /// Count of used elements in elements vector
    size_t used_count;

    /// Vector for stored elements
    vector<optional<element_type>> elements;

    /**
     * @brief Assert that the requested handle is not deleted or throw an
     *        exception otherwise.
     */
    void check_access(handle_type handle) const;
};

}

#include "stable_vector.tcc"

#endif //TSL_STABLE_VECTOR_HPP
