#include "../util/panic.hpp"

namespace tsl {

template<typename handle_t, typename value_t>
value_t& attribute_map<handle_t, value_t>::operator[](handle_t key)
{
    auto elem = get(key);
    if (!elem)
    {
        panic("attempt to access a non-existing value in an attribute map");
    }
    return *elem;
}

template<typename handle_t, typename value_t>
const value_t& attribute_map<handle_t, value_t>::operator[](handle_t key) const
{
    auto elem = get(key);
    if (!elem)
    {
        panic("attempt to access a non-existing value in an attribute map");
    }
    return *elem;
}

template<typename handle_t, typename value_t>
attribute_map<handle_t, value_t>::~attribute_map() = default;

template<typename handle_t>
attribute_map_iterator_ptr<handle_t>& attribute_map_iterator_ptr<handle_t>::operator++()
{
    ++(*iter);
    return *this;
}

template<typename handle_t>
bool attribute_map_iterator_ptr<handle_t>::operator==(const attribute_map_iterator_ptr& other) const
{
    return *iter == *other.iter;
}

template<typename handle_t>
bool attribute_map_iterator_ptr<handle_t>::operator!=(const attribute_map_iterator_ptr& other) const
{
    return *iter != *other.iter;
}

template<typename handle_t>
handle_t attribute_map_iterator_ptr<handle_t>::operator*() const
{
    return **iter;
}

template<typename handle_t>
attribute_map_iterator_ptr<handle_t>& attribute_map_iterator_ptr<handle_t>::operator=(const attribute_map_iterator_ptr& iterator_ptr) {
    iter = iterator_ptr.iter->clone();
    return *this;
}

template<typename handle_t>
attribute_map_iterator_ptr<handle_t>& attribute_map_iterator_ptr<handle_t>::operator=(attribute_map_iterator_ptr&& iterator_ptr) noexcept {
    iter = std::move(iterator_ptr.iter);
    return *this;
}

template<typename handle_t>
attribute_map_iterator_ptr<handle_t>::attribute_map_iterator_ptr(const attribute_map_iterator_ptr& iterator_ptr) {
    *this = iterator_ptr;
}

template<typename handle_t>
attribute_map_iterator_ptr<handle_t>::attribute_map_iterator_ptr(attribute_map_iterator_ptr&& iterator_ptr) noexcept {
    *this = std::move(iterator_ptr);
}

template<typename handle_t>
attribute_map_iterator_ptr<handle_t>::~attribute_map_iterator_ptr() = default;

template<typename handle_t>
attribute_map_iterator<handle_t>::~attribute_map_iterator() = default;

}
