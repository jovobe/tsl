#include <sstream>
#include <functional>

#include <tsl/util/panic.hpp>

using std::nullopt;
using std::reference_wrapper;

namespace tsl {

template<typename handle_t, typename elem_t>
void stable_vector<handle_t, elem_t>::check_access(handle_type handle) const
{
    // Only actually check in debug mode, because checking this is costly...
#ifndef NDEBUG
    // Make sure the handle is not OOB
    if (handle.get_idx() >= size())
    {
        panic("lookup with an out of bounds handle ({}) in stable_vector", handle);
    }

    // You cannot access deleted or uninitialized elements!
    if (!elements[handle.get_idx()])
    {
        panic("attempt to access a deleted value in stable_vector");
    }
#endif
}

template<typename handle_t, typename elem_t>
stable_vector<handle_t, elem_t>::stable_vector(size_t count_elements, const element_type& default_value)
    : elements(count_elements, default_value),
      used_count(count_elements)
{}

template<typename handle_t, typename elem_t>
handle_t stable_vector<handle_t, elem_t>::push(const element_type& elem)
{
    elements.emplace_back(elem);
    ++used_count;
    return handle_t(size() - 1);
}

template<typename handle_t, typename elem_t>
handle_t stable_vector<handle_t, elem_t>::push(element_type&& elem)
{
    elements.emplace_back(move(elem));
    ++used_count;
    return handle_t(size() - 1);
}

template<typename handle_t, typename elem_t>
void stable_vector<handle_t, elem_t>::increase_size(handle_type up_to)
{
    if (up_to.get_idx() < size())
    {
        panic("call to increase_size() with a valid handle!");
    }

    elements.resize(up_to.get_idx(), nullopt);
}

template<typename handle_t, typename elem_t>
void stable_vector<handle_t, elem_t>::increase_size(handle_type up_to, const element_type& elem)
{
    if (up_to.get_idx() < size())
    {
        panic("call to increase_size() with a valid handle!");
    }

    elements.resize(up_to.get_idx(), elem);
}

template <typename handle_t, typename elem_t>
handle_t stable_vector<handle_t, elem_t>::next_handle() const
{
    return handle_t(size());
}

template<typename handle_t, typename elem_t>
void stable_vector<handle_t, elem_t>::erase(handle_type handle)
{
    check_access(handle);

    elements[handle.get_idx()] = nullopt;
    --used_count;
}

template<typename handle_t, typename elem_t>
void stable_vector<handle_t, elem_t>::clear()
{
    elements.clear();
    used_count = 0;
}

template<typename handle_t, typename elem_t>
optional<reference_wrapper<elem_t>> stable_vector<handle_t, elem_t>::get(handle_type handle)
{
    if (handle.get_idx() >= size() || !elements[handle.get_idx()])
    {
        return nullopt;
    }
    return *elements[handle.get_idx()];
}

template<typename handle_t, typename elem_t>
optional<reference_wrapper<const elem_t>> stable_vector<handle_t, elem_t>::get(handle_type handle) const
{
    if (handle.get_idx() >= size() || !elements[handle.get_idx()])
    {
        return nullopt;
    }
    return *elements[handle.get_idx()];
}

template<typename handle_t, typename elem_t>
elem_t& stable_vector<handle_t, elem_t>::operator[](handle_type handle)
{
    check_access(handle);
    return *elements[handle.get_idx()];
}

template<typename handle_t, typename elem_t>
const elem_t& stable_vector<handle_t, elem_t>::operator[](handle_type handle) const
{
    check_access(handle);
    return *elements[handle.get_idx()];
}

template<typename handle_t, typename elem_t>
size_t stable_vector<handle_t, elem_t>::size() const
{
    return elements.size();
}

template<typename handle_t, typename elem_t>
size_t stable_vector<handle_t, elem_t>::num_used() const
{
    return used_count;
}

template<typename handle_t, typename elem_t>
void stable_vector<handle_t, elem_t>::set(handle_type handle, const element_type& elem)
{
    // check access
    if (handle.get_idx() >= size())
    {
        panic("attempt to append new element in stable_vector with set() -> use push()!");
    }

    // insert element
    if (!elements[handle.get_idx()])
    {
        ++used_count;
    }
    elements[handle.get_idx()] = elem;
};

template<typename handle_t, typename elem_t>
void stable_vector<handle_t, elem_t>::set(handle_type handle, element_type&& elem)
{
    // check access
    if (handle.get_idx() >= size())
    {
        panic("attempt to append new element in stable_vector with set() -> use push()!");
    }

    // insert element
    if (!elements[handle.get_idx()])
    {
        ++used_count;
    }
    elements[handle.get_idx()] = elem;
};

template<typename handle_t, typename elem_t>
void stable_vector<handle_t, elem_t>::reserve(size_t new_cap)
{
    elements.reserve(new_cap);
};

template<typename handle_t, typename elem_t>
stable_vector_iterator<handle_t, elem_t> stable_vector<handle_t, elem_t>::begin() const
{
    return stable_vector_iterator<handle_t, elem_t>(&this->elements);
}

template<typename handle_t, typename elem_t>
stable_vector_iterator<handle_t, elem_t> stable_vector<handle_t, elem_t>::end() const
{
    return stable_vector_iterator<handle_t, elem_t>(&this->elements, true);
}

template<typename handle_t, typename elem_t>
stable_vector_iterator<handle_t, elem_t>::stable_vector_iterator(
    const vector<optional<elem_t>>* deleted,
    bool start_at_end
)
    : elements(deleted), pos(start_at_end ? deleted->size() : 0)
{
    if (pos == 0 && !elements->empty() && !(*elements)[0])
    {
        ++(*this);
    }
}

template<typename handle_t, typename elem_t>
stable_vector_iterator<handle_t, elem_t>& stable_vector_iterator<handle_t, elem_t>::operator=(
    const stable_vector_iterator<handle_t, elem_t>& other
)
{
    if (&other == this)
    {
        return *this;
    }
    pos = other.pos;
    elements = other.elements;

    return *this;
}

template<typename handle_t, typename elem_t>
bool stable_vector_iterator<handle_t, elem_t>::operator==(
    const stable_vector_iterator<handle_t, elem_t>& other
) const
{
    return pos == other.pos && elements == other.elements;
}

template<typename handle_t, typename elem_t>
bool stable_vector_iterator<handle_t, elem_t>::operator!=(
    const stable_vector_iterator<handle_t, elem_t>& other
) const
{
    return !(*this == other);
}

template<typename handle_t, typename elem_t>
stable_vector_iterator<handle_t, elem_t>& stable_vector_iterator<handle_t, elem_t>::operator++()
{
    // If not at the end, advance by one element
    if (pos < elements->size())
    {
        pos++;
    }

    // Advance until the next element, at most 1 element behind the vector, to
    // indicate the end of iteration.
    while (pos < elements->size() && !(*elements)[pos])
    {
        pos++;
    }

    return *this;
}

template<typename handle_t, typename elem_t>
bool stable_vector_iterator<handle_t, elem_t>::is_at_end() const
{
    return pos == elements->size();
}

template<typename handle_t, typename elem_t>
handle_t stable_vector_iterator<handle_t, elem_t>::operator*() const
{
    return handle_t(pos);
}

}
