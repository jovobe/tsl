#ifndef TSL_SURFACE_HPP
#define TSL_SURFACE_HPP

#include <utility>
#include <tuple>

#include "tsl/geometry/rectangle.hpp"
#include "tsl/attrmaps/attr_maps.hpp"
#include "tsl/geometry/transform.hpp"
#include "tsl/geometry/tmesh/tmesh.hpp"
#include "tsl/grid.hpp"

using std::tuple;
using std::move;

namespace tsl {

enum class tag {
    positive_u,
    negative_v
};

struct local_knot_vectors {
    vector<double> u;
    vector<double> v;

    local_knot_vectors(vector<double>&& u, vector<double>&& v): u(move(u)), v(move(v)) {}
};

struct evaluator_config {
    bool panic_at_integrity_violations;

    evaluator_config() : panic_at_integrity_violations(false) {}
};

/// uv
using coord_map = dense_half_edge_map<vec2>;
/// dir
using dir_map = dense_half_edge_map<uint8_t>;
/// t(h)
using edge_trans_map = dense_half_edge_map<transform>;
/// t(J^a_i)
using basis_fun_trans_map = dense_vertex_map<vector<transform>>;
/// J^a_i
using basis_fun_map = dense_vertex_map<vector<tuple<half_edge_handle, tag>>>;
/// k^a_i,{1,2}
using knot_map = dense_vertex_map<vector<array<double, 2>>>;
/// C_m
using support_map = dense_face_map<vector<tuple<vertex_handle, index, transform>>>;

/**
 * @brief
 */
class surface_evaluator {
public:
    explicit surface_evaluator(tmesh&& mesh);

    surface_evaluator(const surface_evaluator&) = delete;
    surface_evaluator(surface_evaluator&& evaluator) = default;
    surface_evaluator& operator=(const surface_evaluator&) = delete;
    surface_evaluator& operator=(surface_evaluator&& evaluator) = default;
    ~surface_evaluator() = default;

    // TODO: Implement `surface_evaluator::eval`
    regular_grid eval(uint32_t res) const;
    vector<regular_grid> eval_per_face(uint32_t res) const;

    regular_grid eval_bsplines(uint32_t res, face_handle handle) const;
    array<vec3, 3> eval_bsplines_point(double u, double v, face_handle f) const;
    regular_grid eval_subdevision(uint32_t res, face_handle handle) const;

    /**
     * @brief The max u and v coordinates for the local system of the given face returned as (u, v).
     */
    vec2 get_max_coords(face_handle handle) const;

    tmesh& get_tmesh();

    // ========================================================================
    // = T-Mesh modifier
    // ========================================================================

    /**
     * @see remove_edges(tmesh&, double)
     */
    size_t remove_edges(double percent);

    /**
     * @see tmesh::remove_edge(edge_handle)
     */
    bool remove_edge(edge_handle handle);

    /**
     * @see tmesh::get_vertex_position(vertex_handle)
     */
    vec3& get_vertex_pos(vertex_handle handle);

    evaluator_config config;

    const support_map& get_support_map() const { return support; }
    const dense_vertex_map<vector<local_knot_vectors>>& get_knot_vectors() const { return knot_vectors; }
    const coord_map& get_coord_map() const { return uv; }
    const dir_map& get_dir_map() const { return dir; }
    const edge_trans_map& get_edge_trans_map() const { return edge_trans; }

    /**
     * @brief Updates the local cached values, because the mesh structure has changed.
     *
     * This could be updated to only update the cache for some values instead of updating the whole cache.
     */
    void update_cache();
private:
    tmesh mesh;
    coord_map uv;
    dir_map dir;
    edge_trans_map edge_trans;
    support_map support;
    knot_map knots;
    basis_fun_map handles;
    dense_vertex_map<vector<local_knot_vectors>> knot_vectors;

    // TODO: this will be removed, when evaluation near borders is implemented
    inline static const string EXPECT_NO_BORDER = "tried to determine support of basis functions for border face - this is not implemented!";

    // ========================================================================
    // = Helper functions
    // ========================================================================

    /**
     * @brief Finds the control vertices needed for subdevision surface evaluation for the given face.
     */
    vector<vertex_handle> get_vertices_for_subd(face_handle handle) const;

    /**
     * @brief Returns the parametric domain for the given basis function handle represented as the vertex and the index
     *        as an axis aligned rectangle.
     */
    aa_rectangle get_parametric_domain(vertex_handle handle, size_t handle_index) const;

    /**
     * @brief Calculates the knot vectors in u and v direction for the basis function rooted at the given vertex.
     */
    local_knot_vectors get_knot_vectors(vertex_handle handle, size_t handle_index) const;

    /**
     * @brief Depending on the configuration, print error or panic.
     */
    void report_error(const string& msg) const;

    // ========================================================================
    // = Routines from paper
    // ========================================================================

    /**
     * @brief C.1 (determine_local_coordinate_systems)
     */
    void calc_local_coords();

    /**
     * @brief C.2 (determine_edge_transitions)
     */
    void calc_edge_trans();

    /**
     * @brief C.3 (setup_basis_function_handles_and_transitions)
     */
    basis_fun_trans_map setup_basis_funs();

    /**
     * @brief C.4 (determine_knot_vectors)
     */
    void calc_knots();

    /**
     * @brief C.5 (determine_support_of_basis_functions)
     */
    void calc_support(const basis_fun_trans_map& transforms);
};

}

#endif //TSL_SURFACE_HPP
