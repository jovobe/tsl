#ifndef TSL_SUBDEVISION_HPP
#define TSL_SUBDEVISION_HPP

#include <vector>

#include "tsl/geometry/tmesh/handles.hpp"
#include "tsl/attrmaps/stable_vector.hpp"

using std::vector;

namespace tsl {

inline static const uint32_t MAX_VALENCE = 64;

/**
 * @brief Represents an entry with eigenvalues for subd_eval.
 */
struct eigen_struct {
    int N_;
    int twoN_;
    int K_;
    int M_;
    vector<double> V_;
    vector<double> L_;
    vector<double> iV_;
    bool loaded_;

    eigen_struct() : N_(0), twoN_(0), K_(0), M_(0), loaded_(false) {}
};

/**
 * @brief Represents a handle for an entry in the `eigen_cache`.
 */
class eigen_handle : public base_handle<index> {
    using base_handle<index>::base_handle;
};

/**
 * @brief A cache for the eigen_struct, which is loaded from the files in the `eigenvalues` folder. The key of
 * this cache is the valence of a vertex and the value of the key is the loaded eigen_struct.
 */
class eigen_cache {
public:
    eigen_cache() : cache(MAX_VALENCE, eigen_struct()) {}
    const eigen_struct& get(eigen_handle handle);

private:
    static eigen_struct load(index valence);
    stable_vector<eigen_handle, eigen_struct> cache;
};

/**
 * @brief Evaluates a point of the given surface with the subdevision surface evaluation from a paper from Stam, called
 * "Exact Evaluation Of Catmull-Clark Subdivision Surfaces At Arbitrary Parameter Values".
 * @param u The u position on the surface.
 * @param v The v position on the surface.
 * @param K Number of control points of the surface.
 * @param Cx X values of the control points.
 * @param Cy Y values of the control points.
 * @param Cz Z values of the control points.
 * @param pt Evaluated points (x, y, z).
 * @param du Evaluated points (x, y, z) of the first derivative in u direction.
 * @param dv Evaluated points (x, y, z) of the first derivative in v direction.
 * @param duu ???
 * @param duv ???
 * @param dvv ???
 */
void subd_eval(double& u,
               double& v,
               int K,
               const double* Cx,
               const double* Cy,
               const double* Cz,
               double* pt,
               double* du,
               double* dv,
               double* duu,
               double* duv,
               double* dvv);

}

handle_formatter(tsl::eigen_handle, "EI")

#endif //TSL_SUBDEVISION_HPP
