#ifndef TSL_TSPLINES_HPP
#define TSL_TSPLINES_HPP

#include <tuple>
#include <unordered_map>
#include <array>
#include <variant>

#include <glm/glm.hpp>

#include <tsl/geometry/half_edge_mesh.hpp>
#include <tsl/attrmaps/attr_maps.hpp>
#include <tsl/grid.hpp>
#include <tsl/geometry/rectangle.hpp>

using std::tuple;
using std::unordered_map;
using std::hash;
using std::array;
using std::pair;

using glm::vec2;

namespace tsl {

struct indexed_vertex_handle {
    vertex_handle vertex;
    uint32_t index;

    indexed_vertex_handle(vertex_handle vertex, uint32_t index) : vertex(vertex), index(index) {}

    bool operator==(const indexed_vertex_handle& other) const;
    bool operator!=(const indexed_vertex_handle& other) const;
};

struct double_indexed_vertex_handle {
    vertex_handle vertex;
    uint32_t vertex_index;
    uint8_t knot_index;

    double_indexed_vertex_handle(vertex_handle vertex, uint32_t vertex_index, uint8_t knot_index) : vertex(vertex), vertex_index(vertex_index), knot_index(knot_index) {}

    bool operator==(const double_indexed_vertex_handle& other) const;
    bool operator!=(const double_indexed_vertex_handle& other) const;
};

}

namespace std {

// TODO: check if this hash is good enough
template<>
struct hash<tsl::indexed_vertex_handle> {
    size_t operator()(const tsl::indexed_vertex_handle& h) const {
        return hash<size_t>()(h.vertex.get_idx() ^ h.index);
    }
};

// TODO: check if this hash is good enough
template<>
struct hash<tsl::double_indexed_vertex_handle> {
    size_t operator()(const tsl::double_indexed_vertex_handle& h) const {
        return hash<size_t>()(h.vertex.get_idx() ^ h.vertex_index ^ h.knot_index);
    }
};

}

namespace tsl {

struct transform {
    float f;
    uint8_t r;
    vec2 t;

    transform(float f, uint8_t r, vec2 t) : f(f), r(r), t(t) {}

    transform apply(const transform& trans) const;
    vec2 apply(const vec2& vec) const;
};

enum class tag {
    positive_u,
    negative_v
};

struct tmesh {

    inline static const string EXPECT_NO_BORDER = "tried to determine support of basis functions for border face - this is not implemented!";
    // TODO: Make degree dynamic
    inline static const uint8_t DEGREE = 3;

    half_edge_mesh mesh;
    dense_half_edge_map<float> knots;
    dense_half_edge_map<bool> corners;

    dense_face_map<vector<tuple<indexed_vertex_handle, transform>>> support_map;
    unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>> handles;
    unordered_map<double_indexed_vertex_handle, float> knot_vectors;

    tuple<dense_half_edge_map<vec2>, dense_half_edge_map<uint8_t>> determine_local_coordinate_systems() const;

    dense_half_edge_map<transform>
    determine_edge_transitions(const dense_half_edge_map<vec2>& uv, const dense_half_edge_map<uint8_t>& dir) const;

    tuple<unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>>, unordered_map<indexed_vertex_handle, transform>>
    setup_basis_function_handles_and_transitions(const dense_half_edge_map<vec2>& uv,
                                                 const dense_half_edge_map<uint8_t>& dir) const;

    unordered_map<double_indexed_vertex_handle, float>
    determine_knot_vectors(const unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>>& handles) const;

    dense_face_map<vector<tuple<indexed_vertex_handle, transform>>>
    determine_support_of_basis_functions(
        const unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>>& handles,
        const unordered_map<indexed_vertex_handle, transform>& transforms,
        const dense_half_edge_map<transform>& edge_transforms,
        const dense_half_edge_map<vec2>& uv,
        const unordered_map<double_indexed_vertex_handle, float>& knot_vectors
    ) const;

    float fac(half_edge_handle handle) const;
    bool from_corner(half_edge_handle handle) const;
    aa_rectangle get_parametric_domain(
        const indexed_vertex_handle& handle,
        const unordered_map<double_indexed_vertex_handle, float>& knot_vectors,
        const unordered_map<indexed_vertex_handle, tuple<half_edge_handle, tag>>& handles
    ) const;

    /**
     * @brief Returns the knot vectors {u, v} for the given handle.
     */
    pair<array<float, 5>, array<float, 5>> get_knot_vectors(const indexed_vertex_handle& handle) const;

    /**
     * @brief Returns the index from the given handle increased by the given offset.
     *
     * The returned index is wrapped by the valence of the vertex belonging to the given handle.
     */
    uint32_t get_wrapped_offset_index(const indexed_vertex_handle& handle, int32_t offset) const;
    uint32_t get_extended_valence(const vertex_handle handle) const;
    bool is_extraordinary(const vertex_handle handle) const;

    vector<regular_grid> get_grids(uint32_t resolution) const;
    regular_grid get_grid(uint32_t resolution) const;
    vec3 get_surface_point_of_face(float u, float v, face_handle f) const;
};

/**
 * Applies the following multiplication on the given vector:
 * R^times * vec, where R is the matrix: [[0,-1],[1,0]]
 */
vec2 rotate(uint8_t times, const vec2& vec);

struct tsplines {
    static float get_basis_fun(float u, const array<float, 5>& knot_vector);
    static optional<uint8_t> get_span(float u, const array<float, 5>& knot_vector);

    static tmesh get_example_data_1();
    static tmesh get_example_data_2(uint32_t size);
};

}

#endif //TSL_TSPLINES_HPP
