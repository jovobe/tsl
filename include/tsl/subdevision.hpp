#ifndef TSL_SUBDEVISION_HPP
#define TSL_SUBDEVISION_HPP

#include <vector>

#include <tsl/geometry/handles.hpp>
#include <tsl/attrmaps/stable_vector.hpp>

namespace tsl {

inline static const uint32_t MAX_VALENCE = 64;

struct eigen_struct {
    int N_;
    int twoN_;
    int K_;
    int M_;
    std::vector<double> V_;
    std::vector<double> L_;
    std::vector<double> iV_;
    bool loaded_;

    eigen_struct() : N_(0), twoN_(0), K_(0), M_(0), loaded_(false) {}
};

class eigen_handle : public base_handle<index> {
    using base_handle<index>::base_handle;
};

class eigen_cache {
public:
    eigen_cache() : cache(MAX_VALENCE, eigen_struct()) {}
    const eigen_struct& get(eigen_handle handle);

private:
    static eigen_struct load(index valence);
    stable_vector<eigen_handle, eigen_struct> cache;
};

void subd_eval(double& u, //parametric u value
          double& v, //parametric v value
          int K,   //number of control points
          const double* Cx,//control points in x
          const double* Cy,//control points in y
          const double* Cz,//control points in z
          double* pt,      //results
          double* du,
          double* dv,
          double* duu,
          double* duv,
          double* dvv);

}

handle_formatter(eigen_handle, "EI")

#endif //TSL_SUBDEVISION_HPP
