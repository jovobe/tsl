#include <tsl/subdevision.hpp>

#include <fstream>
#include <string>
#include <cmath>
#include <cblas.h>
#include <limits>

namespace tsl {

double M_[16] = {(-1./6), (3./6), (-3./6), (1./6), (3./6), (-6./6), (3./6), (.0/6),
                 (-3./6), (.0/6), (3./6), (.0/6), (1./6), (4./6), (1./6), (.0/6)};

const eigen_struct& eigen_cache::get(eigen_handle handle) {
#ifndef NDEBUG
    // Make sure the given handle (valence) is between (inklusive) 3 and MAX_VALENCE.
    if (handle.get_idx() < 3 || handle.get_idx() > MAX_VALENCE)
    {
        stringstream ss;
        ss << "The given valence (" << handle.get_idx() << ") is lower than 3 or higher than the defined maximum (" << MAX_VALENCE << ")!";
        panic(ss.str());
    }
#endif

    auto& out = (*cache.get(handle)).get();

    // If this is not loaded, we got the default element and need to insert it!
    if (!out.loaded_) {
        auto val = load(handle.get_idx());
        out = (cache[handle] = val);
    }

    return out;
}

eigen_struct eigen_cache::load(index valence) {
    std::string path ("eigenvalues/");

    eigen_struct out;

    std::string Vpath = path;
    Vpath += "AeigensV_";
    Vpath += std::to_string(valence);
    Vpath += ".txt";
    std::fstream is;
    is.open (Vpath.c_str (), std::ios::in);

    out.N_ = static_cast<int>(valence);
    out.twoN_ = 2*out.N_;
    out.K_ = out.twoN_ + 8;
    out.M_ = out.K_+9;
    out.loaded_ = true;

    int    size = 0;
    is >> size;
    out.V_.resize (size);
    double val;
    for (int j = 0; j < size; ++j) {
        is >> val;
        out.V_[j] = val;
    }
    is.close ();

    std::string Lpath = path;
    Lpath += "AeigensL_";
    Lpath += std::to_string(valence);
    Lpath += ".txt";
    is.open (Lpath.c_str (), std::ios::in);

    is >> size;
    out.L_.resize (size);
    for (int j = 0; j < size; ++j) {
        is >> val;
        out.L_[j] = val;
    }
    is.close ();

    std::string iVpath = path;
    iVpath += "AeigensIV_";
    iVpath += std::to_string(valence);
    iVpath += ".txt";
    is.open (iVpath.c_str (), std::ios::in);

    is >> size;
    out.iV_.resize (size);
    for (int j = 0; j < size; ++j) {
        is >> val;
        out.iV_[j] = val;
    }
    is.close ();

    return out;
}

void subd_eval (
    double& u, //parametric u value
    double& v, //parametric v value
    const int K,   //number of control points
    const double* Cx,//control points in x
    const double* Cy,//control points in y
    const double* Cz,//control points in z
    double* pt,      //results
    double* du,
    double* dv,
    double* duu,
    double* duv,
    double* dvv)
{
    const int twoN = K-8;
    const int N = twoN / 2;
    const int M = K+9;

    static eigen_cache eigens;
    const auto& eigen = eigens.get(eigen_handle(static_cast<index>(N)));

    std::vector<double> CiVx(K), CiVy(K), CiVz(K), VLnCiVx(M), VLnCiVy(M), VLnCiVz(M);

    double Gx[16], Gy[16], Gz[16],
        GxMt[16], GyMt[16], GzMt[16],
        MGxMt[16], MGyMt[16], MGzMt[16],
        Uknots[4], Vknots[4], Up[4], Vp[4],
        bx[4], by[4], bz[4];

    const double alpha = 1., beta = .0;
    const int INC = 1;
    const std::vector< double >& iV = eigen.iV_;

    cblas_dgemv (CblasColMajor, CblasNoTrans, K, K, alpha, &(iV[0]), K, &(Cx[0]), INC, beta, &(CiVx[0]), INC);
    cblas_dgemv (CblasColMajor, CblasNoTrans, K, K, alpha, &(iV[0]), K, &(Cy[0]), INC, beta, &(CiVy[0]), INC);
    cblas_dgemv (CblasColMajor, CblasNoTrans, K, K, alpha, &(iV[0]), K, &(Cz[0]), INC, beta, &(CiVz[0]), INC);

    if (u == 0)
        u = 1e-15;
    if (v == 0)
        v = 1e-15;
    uint32_t k, n = (uint32_t) floor (std::min (-std::log2 (u), -std::log2 (v)));
    n = std::min(n, static_cast<uint32_t>(floor(std::log2(std::numeric_limits<uint32_t>::max()))));

    uint32_t pow2 = 1 << n;
    u *= pow2;
    v *= pow2;
    if (v < 0.5) {
        k = 1; u = 2*u-1; v = 2*v;
    }
    else if (u < 0.5) {
        k = 3; u = 2*u; v = 2*v-1;
    }
    else {
        k = 2; u = 2*u-1; v = 2*v-1;
    }

    //compute basis functions
    Uknots[3] = 1.; Vknots[3] = 1.;
    Uknots[2] = u; Vknots[2] = v;
    Uknots[1] = u*u; Vknots[1] = v*v;
    Uknots[0] = Uknots[1]*u; Vknots[0] = Vknots[1]*v;
    Up[3] = .0; Vp[3] = .0;
    Up[2] = 1.; Vp[2] = 1.;
    Up[1] = 2*u; Vp[1] = 2*v;
    Up[0] = 3*Uknots[1]; Vp[0] = 3*Vknots[1];

    if (n) {
        const double * lamdas = &(eigen.L_[0]);
        for (int i = 0; i < K; ++i) {
            double lambdaN = pow (lamdas[i], n);
            CiVx[i] *= lambdaN;
            CiVy[i] *= lambdaN;
            CiVz[i] *= lambdaN;
        }
    }

    const std::vector<double>& V = eigen.V_;
    cblas_dgemv (CblasColMajor, CblasNoTrans, M, K, alpha, &(V[0]), M, &(CiVx[0]), INC, beta, &(VLnCiVx[0]), INC);
    cblas_dgemv (CblasColMajor, CblasNoTrans, M, K, alpha, &(V[0]), M, &(CiVy[0]), INC, beta, &(VLnCiVy[0]), INC);
    cblas_dgemv (CblasColMajor, CblasNoTrans, M, K, alpha, &(V[0]), M, &(CiVz[0]), INC, beta, &(VLnCiVz[0]), INC);

    //Permute based on sub-domain
    switch (k) {

        case 1: {

            Gx[0] = VLnCiVx[((N == 3) ? 1 : 7)];
            Gx[1] = VLnCiVx[6];
            Gx[2] = VLnCiVx[twoN+4];
            Gx[3] = VLnCiVx[twoN+12];
            Gx[4] = VLnCiVx[0];
            Gx[5] = VLnCiVx[5];
            Gx[6] = VLnCiVx[twoN+3];
            Gx[7] = VLnCiVx[twoN+11];
            Gx[8] = VLnCiVx[3];
            Gx[9] = VLnCiVx[4];
            Gx[10] = VLnCiVx[twoN+2];
            Gx[11] = VLnCiVx[twoN+10];
            Gx[12] = VLnCiVx[twoN+6];
            Gx[13] = VLnCiVx[twoN+5];
            Gx[14] = VLnCiVx[twoN+1];
            Gx[15] = VLnCiVx[twoN+9];

            Gy[0] = VLnCiVy[((N == 3) ? 1 : 7)];
            Gy[1] = VLnCiVy[6];
            Gy[2] = VLnCiVy[twoN+4];
            Gy[3] = VLnCiVy[twoN+12];
            Gy[4] = VLnCiVy[0];
            Gy[5] = VLnCiVy[5];
            Gy[6] = VLnCiVy[twoN+3];
            Gy[7] = VLnCiVy[twoN+11];
            Gy[8] = VLnCiVy[3];
            Gy[9] = VLnCiVy[4];
            Gy[10] = VLnCiVy[twoN+2];
            Gy[11] = VLnCiVy[twoN+10];
            Gy[12] = VLnCiVy[twoN+6];
            Gy[13] = VLnCiVy[twoN+5];
            Gy[14] = VLnCiVy[twoN+1];
            Gy[15] = VLnCiVy[twoN+9];

            Gz[0] = VLnCiVz[((N == 3) ? 1 : 7)];
            Gz[1] = VLnCiVz[6];
            Gz[2] = VLnCiVz[twoN+4];
            Gz[3] = VLnCiVz[twoN+12];
            Gz[4] = VLnCiVz[0];
            Gz[5] = VLnCiVz[5];
            Gz[6] = VLnCiVz[twoN+3];
            Gz[7] = VLnCiVz[twoN+11];
            Gz[8] = VLnCiVz[3];
            Gz[9] = VLnCiVz[4];
            Gz[10] = VLnCiVz[twoN+2];
            Gz[11] = VLnCiVz[twoN+10];
            Gz[12] = VLnCiVz[twoN+6];
            Gz[13] = VLnCiVz[twoN+5];
            Gz[14] = VLnCiVz[twoN+1];
            Gz[15] = VLnCiVz[twoN+9];
            break;

        }

        case 2: {

            Gx[0] = VLnCiVx[0];
            Gx[1] = VLnCiVx[5];
            Gx[2] = VLnCiVx[twoN+3];
            Gx[3] = VLnCiVx[twoN+11];
            Gx[4] = VLnCiVx[3];
            Gx[5] = VLnCiVx[4];
            Gx[6] = VLnCiVx[twoN+2];
            Gx[7] = VLnCiVx[twoN+10];
            Gx[8] = VLnCiVx[twoN+6];
            Gx[9] = VLnCiVx[twoN+5];
            Gx[10] = VLnCiVx[twoN+1];
            Gx[11] = VLnCiVx[twoN+9];
            Gx[12] = VLnCiVx[twoN+15];
            Gx[13] = VLnCiVx[twoN+14];
            Gx[14] = VLnCiVx[twoN+13];
            Gx[15] = VLnCiVx[twoN+8];

            Gy[0] = VLnCiVy[0];
            Gy[1] = VLnCiVy[5];
            Gy[2] = VLnCiVy[twoN+3];
            Gy[3] = VLnCiVy[twoN+11];
            Gy[4] = VLnCiVy[3];
            Gy[5] = VLnCiVy[4];
            Gy[6] = VLnCiVy[twoN+2];
            Gy[7] = VLnCiVy[twoN+10];
            Gy[8] = VLnCiVy[twoN+6];
            Gy[9] = VLnCiVy[twoN+5];
            Gy[10] = VLnCiVy[twoN+1];
            Gy[11] = VLnCiVy[twoN+9];
            Gy[12] = VLnCiVy[twoN+15];
            Gy[13] = VLnCiVy[twoN+14];
            Gy[14] = VLnCiVy[twoN+13];
            Gy[15] = VLnCiVy[twoN+8];

            Gz[0] = VLnCiVz[0];
            Gz[1] = VLnCiVz[5];
            Gz[2] = VLnCiVz[twoN+3];
            Gz[3] = VLnCiVz[twoN+11];
            Gz[4] = VLnCiVz[3];
            Gz[5] = VLnCiVz[4];
            Gz[6] = VLnCiVz[twoN+2];
            Gz[7] = VLnCiVz[twoN+10];
            Gz[8] = VLnCiVz[twoN+6];
            Gz[9] = VLnCiVz[twoN+5];
            Gz[10] = VLnCiVz[twoN+1];
            Gz[11] = VLnCiVz[twoN+9];
            Gz[12] = VLnCiVz[twoN+15];
            Gz[13] = VLnCiVz[twoN+14];
            Gz[14] = VLnCiVz[twoN+13];
            Gz[15] = VLnCiVz[twoN+8];
            break;

        }

        case 3: {

            Gx[0] = VLnCiVx[1];
            Gx[1] = VLnCiVx[0];
            Gx[2] = VLnCiVx[5];
            Gx[3] = VLnCiVx[twoN+3];
            Gx[4] = VLnCiVx[2];
            Gx[5] = VLnCiVx[3];
            Gx[6] = VLnCiVx[4];
            Gx[7] = VLnCiVx[twoN+2];
            Gx[8] = VLnCiVx[twoN+7];
            Gx[9] = VLnCiVx[twoN+6];
            Gx[10] = VLnCiVx[twoN+5];
            Gx[11] = VLnCiVx[twoN+1];
            Gx[12] = VLnCiVx[twoN+16];
            Gx[13] = VLnCiVx[twoN+15];
            Gx[14] = VLnCiVx[twoN+14];
            Gx[15] = VLnCiVx[twoN+13];

            Gy[0] = VLnCiVy[1];
            Gy[1] = VLnCiVy[0];
            Gy[2] = VLnCiVy[5];
            Gy[3] = VLnCiVy[twoN+3];
            Gy[4] = VLnCiVy[2];
            Gy[5] = VLnCiVy[3];
            Gy[6] = VLnCiVy[4];
            Gy[7] = VLnCiVy[twoN+2];
            Gy[8] = VLnCiVy[twoN+7];
            Gy[9] = VLnCiVy[twoN+6];
            Gy[10] = VLnCiVy[twoN+5];
            Gy[11] = VLnCiVy[twoN+1];
            Gy[12] = VLnCiVy[twoN+16];
            Gy[13] = VLnCiVy[twoN+15];
            Gy[14] = VLnCiVy[twoN+14];
            Gy[15] = VLnCiVy[twoN+13];

            Gz[0] = VLnCiVz[1];
            Gz[1] = VLnCiVz[0];
            Gz[2] = VLnCiVz[5];
            Gz[3] = VLnCiVz[twoN+3];
            Gz[4] = VLnCiVz[2];
            Gz[5] = VLnCiVz[3];
            Gz[6] = VLnCiVz[4];
            Gz[7] = VLnCiVz[twoN+2];
            Gz[8] = VLnCiVz[twoN+7];
            Gz[9] = VLnCiVz[twoN+6];
            Gz[10] = VLnCiVz[twoN+5];
            Gz[11] = VLnCiVz[twoN+1];
            Gz[12] = VLnCiVz[twoN+16];
            Gz[13] = VLnCiVz[twoN+15];
            Gz[14] = VLnCiVz[twoN+14];
            Gz[15] = VLnCiVz[twoN+13];
            break;

        }
    }

    //convert to power-basis form
    const int FOUR = 4;
    cblas_dgemm (CblasColMajor, CblasNoTrans, CblasNoTrans, FOUR, FOUR, FOUR, alpha, &(Gx[0]), FOUR, &(M_[0]), FOUR, beta, &(GxMt[0]), FOUR);
    cblas_dgemm (CblasColMajor, CblasTrans, CblasNoTrans, FOUR, FOUR, FOUR, alpha, &(M_[0]), FOUR, &(GxMt[0]), FOUR, beta, &(MGxMt[0]), FOUR);
    cblas_dgemm (CblasColMajor, CblasNoTrans, CblasNoTrans, FOUR, FOUR, FOUR, alpha, &(Gy[0]), FOUR, &(M_[0]), FOUR, beta, &(GyMt[0]), FOUR);
    cblas_dgemm (CblasColMajor, CblasTrans, CblasNoTrans, FOUR, FOUR, FOUR, alpha, &(M_[0]), FOUR, &(GyMt[0]), FOUR, beta, &(MGyMt[0]), FOUR);
    cblas_dgemm (CblasColMajor, CblasNoTrans, CblasNoTrans, FOUR, FOUR, FOUR, alpha, &(Gz[0]), FOUR, &(M_[0]), FOUR, beta, &(GzMt[0]), FOUR);
    cblas_dgemm (CblasColMajor, CblasTrans, CblasNoTrans, FOUR, FOUR, FOUR, alpha, &(M_[0]), FOUR, &(GzMt[0]), FOUR, beta, &(MGzMt[0]), FOUR);

    //evaluate
    cblas_dgemv (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGxMt[0]), FOUR, &(Vknots[0]), INC, beta, &(bx[0]), INC);
    pt[0] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(bx[0]), INC);
    du[0] = cblas_ddot (FOUR, &(Up[0]), INC, &(bx[0]), INC);

    cblas_dgemv  (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGyMt[0]), FOUR, &(Vknots[0]), INC, beta, &(by[0]), INC);
    pt[1] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(by[0]), INC);
    du[1] = cblas_ddot (FOUR, &(Up[0]), INC, &(by[0]), INC);

    cblas_dgemv (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGzMt[0]), FOUR, &(Vknots[0]), INC, beta, &(bz[0]), INC);
    pt[2] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(bz[0]), INC);
    du[2] = cblas_ddot (FOUR, &(Up[0]), INC, &(bz[0]), INC);

    if (duu) {
        std::vector< double > Upp (4);
        Upp[3] = .0;
        Upp[2] = .0;
        Upp[1] = 2.;
        Upp[0] = 3.*u;
        duu[0] = cblas_ddot (FOUR, &(Upp[0]), INC, &(bx[0]), INC);
        duu[1] = cblas_ddot (FOUR, &(Upp[0]), INC, &(by[0]), INC);
        duu[2] = cblas_ddot (FOUR, &(Upp[0]), INC, &(bz[0]), INC);
    }

    cblas_dgemv (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGxMt[0]), FOUR, &(Vp[0]), INC, beta, &(bx[0]), INC);
    dv[0] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(bx[0]), INC);
    cblas_dgemv (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGyMt[0]), FOUR, &(Vp[0]), INC, beta, &(by[0]), INC);
    dv[1] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(by[0]), INC);
    cblas_dgemv (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGzMt[0]), FOUR, &(Vp[0]), INC, beta, &(bz[0]), INC);
    dv[2] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(bz[0]), INC);

    if (duv) {
        duv[0] = cblas_ddot (FOUR, &(Up[0]), INC, &(bx[0]), INC);
        duv[1] = cblas_ddot (FOUR, &(Up[0]), INC, &(by[0]), INC);
        duv[2] = cblas_ddot (FOUR, &(Up[0]), INC, &(bz[0]), INC);
    }

    if (dvv) {
        std::vector< double > Vpp (4);
        Vpp[3] = .0;
        Vpp[2] = .0;
        Vpp[1] = 2.;
        Vpp[0] = 3.*v;
        cblas_dgemv (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGxMt[0]), FOUR, &(Vpp[0]), INC, beta, &(bx[0]), INC);
        dvv[0] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(bx[0]), INC);
        cblas_dgemv (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGyMt[0]), FOUR, &(Vpp[0]), INC, beta, &(by[0]), INC);
        dvv[1] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(by[0]), INC);
        cblas_dgemv (CblasColMajor, CblasNoTrans, FOUR, FOUR, alpha, &(MGzMt[0]), FOUR, &(Vpp[0]), INC, beta, &(bz[0]), INC);
        dvv[2] = cblas_ddot (FOUR, &(Uknots[0]), INC, &(bz[0]), INC);
    }
}

}
