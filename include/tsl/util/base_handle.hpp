#ifndef TSL_BASE_HANDLE_HPP
#define TSL_BASE_HANDLE_HPP

#include <optional>
#include <string>

using std::optional;
using std::string;

namespace tsl {

/**
 * @brief Interface for all kinds of handles. Handles are basically a key to
 *        refer to something.
 *
 * From this class, a few concrete handle types (such as face_handle) will be
 * derived.
 *
 * Internally, the handle is just an index. How those indices are used is
 * determined by the thing creating handles (e.g. the mesh implementation).
 */
template<typename idx_t>
class base_handle
{
public:
    explicit base_handle(idx_t idx);

    idx_t get_idx() const;
    void set_idx(idx_t idx);

    bool operator==(const base_handle& other) const;
    bool operator!=(const base_handle& other) const;

protected:
    idx_t idx;
};

/**
 * @brief Base class for optional handles (handles that can be "null" or
 *        "None").
 *
 * This class is semantically equivalent to optional<base_handle>. This
 * class uses a special index value to store the "None" value. This saves
 * memory.
 */
template <typename idx_t, typename non_optional_t>
class base_optional_handle
{
public:
    base_optional_handle();
    explicit base_optional_handle(base_handle<idx_t> handle);
    explicit base_optional_handle(optional<base_handle<idx_t>> handle);
    explicit base_optional_handle(idx_t idx);

    explicit operator bool() const;

    bool operator!() const;

    bool operator==(const base_optional_handle& other) const;
    bool operator!=(const base_optional_handle& other) const;

    /**
     * @brief Extracts the handle. If `this` doesn't hold a handle (is "None"),
     *        this method panics.
     */
    non_optional_t unwrap() const;

    /**
     * @brief Extracts the handle. If `this` doesn't hold a handle (is "None"),
     *        this method panics with the given msg.
     */
    non_optional_t expect(const string& msg) const;
    void set_idx(idx_t idx);

private:
    idx_t idx;
};

}

#include <tsl/util/base_handle.tcc>

#endif //TSL_BASE_HANDLE_HPP
