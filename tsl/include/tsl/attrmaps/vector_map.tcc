#include <optional>

using std::nullopt;

namespace tsl {

template<typename handle_t, typename value_t>
vector_map<handle_t, value_t>::vector_map(const value_t& default_value)
    : default_value(default_value)
{}

template<typename handle_t, typename value_t>
vector_map<handle_t, value_t>::vector_map(size_t count_elements, const value_t& default_value)
    : default_value(default_value)
{
    reserve(count_elements);
}

template<typename handle_t, typename value_t>
bool vector_map<handle_t, value_t>::contains_key(handle_t key) const
{
    return static_cast<bool>(vec.get(key));
}

template<typename handle_t, typename value_t>
optional<value_t> vector_map<handle_t, value_t>::insert(handle_t key, const value_t& value)
{
    // If the vector isn't large enough yet, we allocate additional space.
    if (key.get_idx() >= vec.size())
    {
        vec.increase_size(key);
        vec.push(value);
        return nullopt;
    }
    else
    {
        auto out = erase(key);
        vec.set(key, value);
        return out;
    }
}

template<typename handle_t, typename value_t>
optional<value_t> vector_map<handle_t, value_t>::erase(handle_t key)
{
    auto val = vec.get(key);
    if (val)
    {
        auto out = value_t(std::move(*val));
        vec.erase(key);
        return out;
    }
    else
    {
        return nullopt;
    }
}

template<typename handle_t, typename value_t>
void vector_map<handle_t, value_t>::clear()
{
    vec.clear();
}

template<typename handle_t, typename value_t>
optional<reference_wrapper<value_t>> vector_map<handle_t, value_t>::get(handle_t key)
{
    // Try to lookup value. If none was found and a default value is set,
    // insert it and return that instead.
    auto res = vec.get(key);
    if (!vec.get(key) && default_value)
    {
        insert(key, *default_value);
        return vec.get(key);
    }
    return res;
}

template<typename handle_t, typename value_t>
optional<reference_wrapper<const value_t>> vector_map<handle_t, value_t>::get(handle_t key) const
{
    // Try to lookup value. If none was found and a default value is set,
    // return that instead.
    auto res = vec.get(key);
    return (!vec.get(key) && default_value) ? *default_value : res;
}

template<typename handle_t, typename value_t>
size_t vector_map<handle_t, value_t>::num_values() const
{
    return vec.num_used();
}


template<typename handle_t, typename value_t>
attribute_map_iterator_ptr<handle_t> vector_map<handle_t, value_t>::begin() const
{
    return attribute_map_iterator_ptr<handle_t>(
        std::make_unique<vector_map_iterator<handle_t, value_t>>(vec.begin())
    );
}

template<typename handle_t, typename value_t>
attribute_map_iterator_ptr<handle_t> vector_map<handle_t, value_t>::end() const
{
    return attribute_map_iterator_ptr<handle_t>(
        std::make_unique<vector_map_iterator<handle_t, value_t>>(vec.end())
    );
}

template<typename handle_t, typename value_t>
void vector_map<handle_t, value_t>::reserve(size_t new_cap)
{
    vec.reserve(new_cap);
}

template<typename handle_t, typename value_t>
vector_map<handle_t, value_t>::~vector_map() = default;


template<typename handle_t, typename value_t>
vector_map_iterator<handle_t, value_t>::vector_map_iterator(stable_vector_iterator<handle_t, value_t> iter)
    : iter(iter)
{}

template<typename handle_t, typename value_t>
attribute_map_iterator<handle_t>& vector_map_iterator<handle_t, value_t>::operator++()
{
    ++iter;
    return *this;
}

template<typename handle_t, typename value_t>
bool vector_map_iterator<handle_t, value_t>::operator==(
    const attribute_map_iterator<handle_t>& other
) const
{
    auto cast = dynamic_cast<const vector_map_iterator<handle_t, value_t>*>(&other);
    return cast && iter == cast->iter;
}

template<typename handle_t, typename value_t>
bool vector_map_iterator<handle_t, value_t>::operator!=(
    const attribute_map_iterator<handle_t>& other
) const
{
    auto cast = dynamic_cast<const vector_map_iterator<handle_t, value_t>*>(&other);
    return !cast || iter != cast->iter;
}

template<typename handle_t, typename value_t>
handle_t vector_map_iterator<handle_t, value_t>::operator*() const
{
    return *iter;
}

template<typename handle_t, typename value_t>
std::unique_ptr<attribute_map_iterator<handle_t>> vector_map_iterator<handle_t, value_t>::clone() const
{
    return std::make_unique<vector_map_iterator>(*this);
}

template<typename handle_t, typename value_t>
vector_map_iterator<handle_t, value_t>::~vector_map_iterator() = default;

}
