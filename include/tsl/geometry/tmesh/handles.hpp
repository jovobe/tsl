#ifndef TSL_HANDLES_HPP
#define TSL_HANDLES_HPP

#include "tsl/util/base_handle.hpp"

using std::hash;

namespace tsl {

/**
 * @brief Datatype used as index for each vertex, face and edge.
 */
using index = uint32_t;

// Note on strongly typed handles:
//
// You might ask: Why do we need that many classes for handles? Wouldn't one
// be enough? Or you go even further: if every handle is just a simple integer,
// why not store the integer directly. Well, it all comes down to "strong
// typing".
//
// Type systems are the main way for compilers to notice that a program is
// faulty. While compiler errors are just annoying in the first few years of
// learning how to program, they become very useful later on. When writing
// software, humans will make mistakes -- that's just a fact. The question is
// WHEN we want to notice those mistakes. There are a few possibilities here:
//
// - while compiling
// - while executing unit tests
// - in production
//
// No one wants to notice bugs when alreay running software in production. Thus
// we want to notice our mistakes earlier. Since this whole library clearly
// doesn't care about unit tests, mistakes can only be noticed either at
// compile time or when the developer executes the program.
//
// Well, now the fun parts. When you use a language with a sufficiently
// powerful type system (as C++) and if you are correctly using this type
// system, you can avoid many huge classes of bugs! The compiler will tell you
// right away, when you made a mistake.
//
// So with these strongly typed handles, you cannot falsly assign an edge_handle
// to a face_handle -- it will result in a compiler error. If you were using
// simple integers, the compiler wouldn't notice and you would have to track
// down the bug manually. Not so great.
//
// Apart from that: it makes reading code so much easier, as you know exactly
// what a specific parameter is for.

/// Handle to access edges of the mesh.
class edge_handle : public base_handle<index> {
    using base_handle<index>::base_handle;
};

/// Handle to access faces of the mesh.
class face_handle : public base_handle<index> {
    using base_handle<index>::base_handle;
};

/// Handle to access vertices of the mesh.
class vertex_handle : public base_handle<index> {
    using base_handle<index>::base_handle;
};

/// Handle to access half edges of the mesh.
class half_edge_handle : public base_handle<index>
{
public:
    using base_handle<index>::base_handle;

    static half_edge_handle one_half_of(edge_handle eh)
    {
        // The index of an edge handle is always the index of the handle of
        // one of its half edges
        return half_edge_handle(eh.get_idx());
    }
};

/// Semantically equivalent to `boost::optional<edge_handle>`
class optional_edge_handle : public base_optional_handle<index, edge_handle> {
    using base_optional_handle<index, edge_handle>::base_optional_handle;
};

/// Semantically equivalent to `boost::optional<face_handle>`
class optional_face_handle : public base_optional_handle<index, face_handle> {
    using base_optional_handle<index, face_handle>::base_optional_handle;
};

/// Semantically equivalent to `boost::optional<vertex_handle>`
class optional_vertex_handle : public base_optional_handle<index, vertex_handle> {
    using base_optional_handle<index, vertex_handle>::base_optional_handle;
};

/// Semantically equivalent to `boost::optional<half_edge_handle>`
class optional_half_edge_handle : public base_optional_handle<index, half_edge_handle>
{
public:
    using base_optional_handle<index, half_edge_handle>::base_optional_handle;
    optional_half_edge_handle() : base_optional_handle() {}
    explicit optional_half_edge_handle(edge_handle eh) : optional_half_edge_handle(eh.get_idx()) {}
};

}

handle_formatter(tsl::edge_handle, "E")
handle_formatter(tsl::face_handle, "F")
handle_formatter(tsl::half_edge_handle, "HE")
handle_formatter(tsl::vertex_handle, "V")

optional_handle_formatter(tsl::optional_edge_handle, "E")
optional_handle_formatter(tsl::optional_face_handle, "F")
optional_handle_formatter(tsl::optional_half_edge_handle, "HE")
optional_handle_formatter(tsl::optional_vertex_handle, "V")

namespace std {

template<>
struct hash<tsl::edge_handle> {
    size_t operator()(const tsl::edge_handle& h) const {
        return hash<tsl::index>()(h.get_idx());
    }
};

template<>
struct hash<tsl::face_handle> {
    size_t operator()(const tsl::face_handle& h) const {
        return hash<tsl::index>()(h.get_idx());
    }
};

template<>
struct hash<tsl::vertex_handle> {
    size_t operator()(const tsl::vertex_handle& h) const {
        return hash<tsl::index>()(h.get_idx());
    }
};

template<>
struct hash<tsl::half_edge_handle> {
    size_t operator()(const tsl::half_edge_handle& h) const {
        return hash<tsl::index>()(h.get_idx());
    }
};

}

#endif //TSL_HANDLES_HPP
