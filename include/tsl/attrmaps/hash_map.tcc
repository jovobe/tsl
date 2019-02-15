#include <utility>
#include <functional>

using std::nullopt;
using std::make_pair;
using std::reference_wrapper;

namespace tsl {

template<typename handle_t, typename value_t>
hash_map<handle_t, value_t>::hash_map(const value_t& default_value)
    : default_value(default_value)
{}

template<typename handle_t, typename value_t>
hash_map<handle_t, value_t>::hash_map(size_t count_elements, const value_t& default_value)
    : default_value(default_value)
{
    reserve(count_elements);
}

template<typename handle_t, typename value_t>
bool hash_map<handle_t, value_t>::contains_key(handle_t key) const
{
    return map.find(key) != map.end();
}

template<typename handle_t, typename value_t>
optional<value_t> hash_map<handle_t, value_t>::insert(handle_t key, const value_t& value)
{
    auto res = map.insert(make_pair(key, value));
    if (!res.second)
    {
        // TODO: this makes some copies that are not necessary. Dunno how
        //       to correctly code this right now. Maybe the compiler optimizes
        //       everything perfectly anyway.
        auto old = (*res.first).second;
        (*res.first).second = value;
        return old;
    }
    else
    {
        return nullopt;
    }
}

template<typename handle_t, typename value_t>
optional<value_t> hash_map<handle_t, value_t>::erase(handle_t key)
{
    auto it = map.find(key);
    if (it != map.end())
    {
        auto out = (*it).second;
        map.erase(it);
        return out;
    }
    else
    {
        return nullopt;
    }
}

template<typename handle_t, typename value_t>
void hash_map<handle_t, value_t>::clear()
{
    map.clear();
}

template<typename handle_t, typename value_t>
optional<reference_wrapper<value_t>> hash_map<handle_t, value_t>::get(handle_t key)
{
    // Try to lookup value. If none was found and a default value is set,
    // insert it and return that instead.
    auto it = map.find(key);
    if (it == map.end())
    {
        if (default_value)
        {
            // Insert default value into hash map and return the inserted value
            auto res = map.insert(make_pair(key, *default_value));
            return (*res.first).second;
        }
        else
        {
            return nullopt;
        }
    }
    return (*it).second;
}

template<typename handle_t, typename value_t>
optional<reference_wrapper<const value_t>> hash_map<handle_t, value_t>::get(handle_t key) const
{
    // Try to lookup value. If none was found and a default value is set,
    // return that instead.
    auto it = map.find(key);
    if (it == map.end())
    {
        if (default_value)
        {
            return *default_value;
        }
        else
        {
            return nullopt;
        }
    }
    return (*it).second;
}

template<typename handle_t, typename value_t>
size_t hash_map<handle_t, value_t>::num_values() const
{
    return map.size();
}

template<typename handle_t, typename value_t>
void hash_map<handle_t, value_t>::reserve(size_t new_cap)
{
    map.reserve(new_cap);
}

template<typename handle_t, typename value_t>
hash_map<handle_t, value_t>::~hash_map() {

}

}
