#ifndef TSL_ITERATOR_HPP
#define TSL_ITERATOR_HPP

#include <type_traits>
#include <memory>

#include "half_edge.hpp"
#include "handles.hpp"
#include "tmesh.hpp"

using std::is_base_of;
using std::unique_ptr;

namespace tsl {

template<typename handle_t>
class tmesh_iterator
{
    static_assert(
        is_base_of<base_handle<index>, handle_t>::value,
        "handle_t must inherit from base_handle!"
    );
public:
    virtual tmesh_iterator& operator++() = 0;
    virtual bool operator==(const tmesh_iterator& other) const = 0;
    virtual bool operator!=(const tmesh_iterator& other) const = 0;
    virtual handle_t operator*() const = 0;
    virtual ~tmesh_iterator() = default;
};

template<typename handle_t>
class tmesh_iterator_ptr
{
public:
    explicit tmesh_iterator_ptr(unique_ptr<tmesh_iterator<handle_t>> iter) : iter(move(iter)) {};
    tmesh_iterator_ptr& operator++();
    bool operator==(const tmesh_iterator_ptr& other) const;
    bool operator!=(const tmesh_iterator_ptr& other) const;
    handle_t operator*() const;

private:
    unique_ptr<tmesh_iterator<handle_t>> iter;
};


// Forward declarations
class tmesh;

class tmesh_face_iterator_proxy
{
public:
    tmesh_iterator_ptr<face_handle> begin() const;
    tmesh_iterator_ptr<face_handle> end() const;

private:
    explicit tmesh_face_iterator_proxy(const tmesh& mesh) : mesh(mesh) {}
    const tmesh& mesh;
    friend tmesh;
};

class tmesh_half_edge_iterator_proxy
{
public:
    tmesh_iterator_ptr<half_edge_handle> begin() const;
    tmesh_iterator_ptr<half_edge_handle> end() const;

private:
    explicit tmesh_half_edge_iterator_proxy(const tmesh& mesh) : mesh(mesh) {}
    const tmesh& mesh;
    friend tmesh;
};

class tmesh_edge_iterator_proxy
{
public:
    tmesh_iterator_ptr<edge_handle> begin() const;
    tmesh_iterator_ptr<edge_handle> end() const;

private:
    explicit tmesh_edge_iterator_proxy(const tmesh& mesh) : mesh(mesh) {}
    const tmesh& mesh;
    friend tmesh;
};

class tmesh_vertex_iterator_proxy
{
public:
    tmesh_iterator_ptr<vertex_handle> begin() const;
    tmesh_iterator_ptr<vertex_handle> end() const;

private:
    explicit tmesh_vertex_iterator_proxy(const tmesh& mesh) : mesh(mesh) {}
    const tmesh& mesh;
    friend tmesh;
};

template<typename handle_t, typename elem_t>
class tmesh_fev_iterator : public tmesh_iterator<handle_t>
{
public:
    explicit tmesh_fev_iterator(stable_vector_iterator<handle_t, elem_t> iterator) : iterator(iterator) {};
    tmesh_fev_iterator& operator++();
    bool operator==(const tmesh_iterator<handle_t>& other) const;
    bool operator!=(const tmesh_iterator<handle_t>& other) const;
    handle_t operator*() const;

private:
    stable_vector_iterator<handle_t, elem_t> iterator;
};

class tmesh_edge_iterator : public tmesh_iterator<edge_handle>
{
public:
    explicit tmesh_edge_iterator(
        stable_vector_iterator<half_edge_handle, half_edge> iterator,
        const tmesh& mesh
    ) : iterator(iterator), mesh(mesh) {};
    tmesh_edge_iterator& operator++();
    bool operator==(const tmesh_iterator<edge_handle>& other) const;
    bool operator!=(const tmesh_iterator<edge_handle>& other) const;
    edge_handle operator*() const;

private:
    stable_vector_iterator<half_edge_handle, half_edge> iterator;
    const tmesh& mesh;
};

}

#include "iterator.tcc"

#endif //TSL_ITERATOR_HPP
