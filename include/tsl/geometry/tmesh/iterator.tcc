namespace tsl {

template<typename handle_t, typename elem_t>
tmesh_fev_iterator<handle_t, elem_t>& tmesh_fev_iterator<handle_t, elem_t>::operator++() {
    ++iterator;
    return *this;
}

template<typename handle_t, typename elem_t>
bool tmesh_fev_iterator<handle_t, elem_t>::operator==(const tmesh_iterator<handle_t>& other) const {
    auto cast = dynamic_cast<const tmesh_fev_iterator<handle_t, elem_t>*>(&other);
    return cast && iterator == cast->iterator;
}

template<typename handle_t, typename elem_t>
bool tmesh_fev_iterator<handle_t, elem_t>::operator!=(const tmesh_iterator<handle_t>& other) const {
    auto cast = dynamic_cast<const tmesh_fev_iterator<handle_t, elem_t>*>(&other);
    return cast && iterator != cast->iterator;
}

template<typename handle_t, typename elem_t>
handle_t tmesh_fev_iterator<handle_t, elem_t>::operator*() const {
    return *iterator;
}

template<typename handle_t>
tmesh_iterator_ptr<handle_t>& tmesh_iterator_ptr<handle_t>::operator++()
{
    ++(*iter);
    return *this;
}

template<typename handle_t>
bool tmesh_iterator_ptr<handle_t>::operator==(const tmesh_iterator_ptr<handle_t>& other) const
{
    return *iter == *other.iter;
}

template<typename handle_t>
bool tmesh_iterator_ptr<handle_t>::operator!=(const tmesh_iterator_ptr<handle_t>& other) const
{
    return *iter != *other.iter;
}

template<typename handle_t>
handle_t tmesh_iterator_ptr<handle_t>::operator*() const
{
    return **iter;
}

}
