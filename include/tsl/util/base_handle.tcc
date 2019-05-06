#include <limits>

#include <tsl/util/panic.hpp>
#include "base_handle.hpp"


using std::numeric_limits;

namespace tsl {

template <typename idx_t>
base_handle<idx_t>::base_handle(idx_t idx)
{
    set_idx(idx);
}

template <typename idx_t>
idx_t base_handle<idx_t>::get_idx() const
{
    return idx;
}
template <typename idx_t>
void base_handle<idx_t>::set_idx(idx_t idx)
{
#ifndef NDEBUG
    if (idx == numeric_limits<idx_t>::max())
    {
        panic("Trying to create a handle with numeric_limits<idx_t>::max() as idx!");
    }
#endif

    this->idx = idx;
}

template <typename idx_t>
bool base_handle<idx_t>::operator==(const base_handle& other) const
{
    return idx == other.idx;
}

template <typename idx_t>
bool base_handle<idx_t>::operator!=(const base_handle& other) const
{
    return idx != other.idx;
}

template<typename idx_t>
bool base_handle<idx_t>::operator<(const base_handle& other) const {
    return idx < other.idx;
}

template <typename idx_t, typename non_optional_t>
base_optional_handle<idx_t, non_optional_t>::base_optional_handle()
    : idx(numeric_limits<idx_t>::max())
{}

template <typename idx_t, typename non_optional_t>
base_optional_handle<idx_t, non_optional_t>::base_optional_handle(base_handle<idx_t> handle)
    : idx(handle.get_idx())
{}

template <typename idx_t, typename non_optional_t>
base_optional_handle<idx_t, non_optional_t>::base_optional_handle(optional<base_handle<idx_t>> handle)
    : idx(handle ? handle->get_idx() : numeric_limits<idx_t>::max())
{}

template <typename idx_t, typename non_optional_t>
base_optional_handle<idx_t, non_optional_t>::base_optional_handle(idx_t idx)
    : base_optional_handle(base_handle<idx_t>(idx))
{}

template <typename idx_t, typename non_optional_t>
base_optional_handle<idx_t, non_optional_t>::operator bool() const
{
    return idx != numeric_limits<idx_t>::max();
}

template <typename idx_t, typename non_optional_t>
bool base_optional_handle<idx_t, non_optional_t>::operator!() const
{
    return idx == numeric_limits<idx_t>::max();
}

template <typename idx_t, typename non_optional_t>
bool base_optional_handle<idx_t, non_optional_t>::operator==(const base_optional_handle& other) const
{
    return idx == other.idx;
}

template <typename idx_t, typename non_optional_t>
bool base_optional_handle<idx_t, non_optional_t>::operator!=(const base_optional_handle& other) const
{
    return idx != other.idx;
}

template <typename idx_t, typename non_optional_t>
non_optional_t base_optional_handle<idx_t, non_optional_t>::unwrap() const
{
    return non_optional_t(idx);
}

template <typename idx_t, typename non_optional_t>
void base_optional_handle<idx_t, non_optional_t>::set_idx(idx_t idx)
{
    assert(idx != numeric_limits<idx_t>::max());
    this->idx = idx;
}

template<typename idx_t, typename non_optional_t>
non_optional_t base_optional_handle<idx_t, non_optional_t>::expect(const string& msg) const {
    if (idx == numeric_limits<idx_t>::max())
    {
        panic(msg);
    }

    return non_optional_t(idx);
}

}
