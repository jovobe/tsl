#ifndef TSL_HANDLES_HPP
#define TSL_HANDLES_HPP

#include <ostream>

#include <tsl/util/base_handle.hpp>

using std::ostream;
using std::hash;

namespace tsl {

/**
 * @brief Datatype used as index for each vertex, face and edge.
 *
 * This index is used within {edge, face, vertex}_handles. Since those
 * handles are also used within each {edge, face, vertex} reducing the
 * size of this type can greatly decrease memory usage, which in
 * turn might increase performance due to cache locality.
 *
 * When we assume a basic half-edge structure, we have to deal with the
 * following struct sizes:
 * - edge: 4 handles
 * - face: 1 handle + 1 vector
 * - vertex: 1 handle + 1 vector
 *
 * Assuming the most common case of `float` vectors, this results in the
 * following sizes (in bytes):
 * - 16 bit handles: edge (8), face (14), vertex (14)
 * - 32 bit handles: edge (16), face (16), vertex (16)
 * - 64 bit handles: edge (32), face (20), vertex (20)
 *
 * Using another approximation of the number of faces, edges and vertices
 * in a triangle-mesh described at [1], we can calculate how much RAM we
 * would need in order to run out of handles. The approximation: for each
 * vertex, we have three edges and two faces. The de-facto cost per vertex
 * can be calculated from that resulting in
 *
 * - 16 bit handles: 14 + 2*14 + 3*8 = 66 bytes/vertex = 22 bytes/edge
 *   ==> 22 * 2^16 = 1.4 MiB RAM necessary to exhaust handle space
 *
 * - 32 bit handles: 16 + 2*16 + 3*16 = 96 bytes/vertex = 32 bytes/edge
 *   ==> 32 * 2^32 = 137 GiB RAM necessary to exhaust handle space
 *
 * - 64 bit handles: 20 + 2*20 + 3*32 = 156 bytes/vertex = 52 bytes/edge
 *   ==> 52 * 2^64 = 1.1 ZiB RAM necessary to exhaust handle space
 *       (it's called zetta or zebi and is ≈ 1 million tera bytes)
 *   ==> Note: funnily enough, the estimated disk (not RAM!) capacity of
 *       the whole  world (around 2015) comes very close to this number.
 *
 *
 * Also note that this accounts for the mesh only and ignores all other
 * data that might need to be stored in RAM. So you will need even more
 * RAM.
 *
 * From this, I think, we can safely conclude: 16 bit handles are way too
 * small; 32 bit handles are probably fine for the next few years, even
 * when working on a medium-sized cluster and 64 bit handles will be fine
 * until after the singularity. And by then, I probably don't care anymore.
 *
 * [1]: https://math.stackexchange.com/q/425968/340615
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

/// Handle to access half edges of the HEM.
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

inline ostream& operator<<(ostream& os, const edge_handle& h) {
    os << "E" << h.get_idx();
    return os;
}

inline ostream& operator<<(ostream& os, const face_handle& h) {
    os << "F" << h.get_idx();
    return os;
}

inline ostream& operator<<(ostream& os, const vertex_handle& h) {
    os << "V" << h.get_idx();
    return os;
}

inline ostream& operator<<(ostream& os, const half_edge_handle& h)
{
    os << "HE" << h.get_idx();
    return os;
}

inline ostream& operator<<(ostream& os, const optional_edge_handle& h) {
    if (h) {
        os << "E" << h.unwrap().get_idx();
    } else {
        os << "E⊥";
    }
    return os;
}

inline ostream& operator<<(ostream& os, const optional_face_handle& h) {
    if (h) {
        os << "F" << h.unwrap().get_idx();
    } else {
        os << "F⊥";
    }
    return os;
}

inline ostream& operator<<(ostream& os, const optional_vertex_handle& h) {
    if (h) {
        os << "V" << h.unwrap().get_idx();
    } else {
        os << "V⊥";
    }
    return os;
}

inline ostream& operator<<(ostream& os, const optional_half_edge_handle& h)
{
    if (h)
    {
        os << "HE" << h.unwrap().get_idx();
    }
    else
    {
        os << "HE⊥";
    }
    return os;
}

}

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
