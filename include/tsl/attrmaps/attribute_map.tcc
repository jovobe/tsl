#include <tsl/util/panic.hpp>

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
attribute_map<handle_t, value_t>::~attribute_map() {

}

}
