#include <tsl/nubs.hpp>

#include <glm/glm.hpp>

#include <algorithm>

using glm::vec3;

using std::min;

namespace tsl {

nubs nubs::get_example_data_1() {

    nubs out;
    out.p = 2;
    out.q = 2;

    // 9 (-> x -> u)
    out.U = {0, 0, 0, 1.0f/5, 1.0f/2, 4.0f/5, 1, 1, 1};
    // 8 (-> y -> v)
    out.V = {0, 0, 0, 2.0f/5, 3.0f/5, 1, 1, 1};

    // grid size: 6 x 5 (x, y)
    out.P.points = {
        {vec3(0, 0, 0), vec3(1, 0, 0), vec3(2, 0, 0), vec3(3, 0, 0), vec3(4, 0, 0), vec3(5, 0, 0)},
        {vec3(0, 1, 0), vec3(1, 1, 0), vec3(2, 1, 0), vec3(3, 1, 0), vec3(4, 1, 0), vec3(5, 1, 0)},
        {vec3(0, 2, 0), vec3(1, 2, 0), vec3(2, 2, 0), vec3(3, 2, 0), vec3(4, 2, 0), vec3(5, 2, 0)},
        {vec3(0, 3, 0), vec3(1, 3, 0), vec3(2, 3, 0), vec3(3, 3, 1), vec3(4, 3, 1.5), vec3(5, 3, 1.5)},
        {vec3(0, 4, 0), vec3(1, 4, 0), vec3(2, 4, 0), vec3(3, 4, 1), vec3(4, 4, 1.5), vec3(5, 4, 1.5)},
    };

    out.x = 6;
    out.y = 5;

    out.n = out.x - 1;
    out.m = out.y - 1;

    return out;
}

nubs nubs::get_example_data_2() {

    nubs out;
    out.p = 2;
    out.q = 2;

    // 6 (-> y -> v)
    out.V = {0, 0, 0, 1, 1, 1};
    // 6 (-> x -> u)
    out.U = {0, 0, 0, 1, 1, 1};

    out.x = 3;
    out.y = 3;

    out.n = out.x - 1;
    out.m = out.y - 1;

    // 3 x 3
    out.P.points = {
        {vec3(0, 0, 0), vec3(1, 0, 0), vec3(2, 0, 0)},
        {vec3(0, 1, 0), vec3(1, 1, 0), vec3(2, 1, 0)},
        {vec3(0, 2, 0), vec3(1, 2, 0), vec3(2, 2, 0.5)},
    };

    return out;
}

nubs nubs::get_example_data_3() {
    nubs out = get_example_data_1();

    out.p = 3;
    out.q = 4;

    out.U = {0, 0, 0, 0, 1.0f/3, 2.0f/3, 1, 1, 1, 1};
    out.V = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1};

    return out;
}

nubs nubs::get_example_data_4() {

    nubs out;
    out.p = 3;
    out.q = 3;

    out.U = {0, 0, 0, 0, 1.0f/2, 1, 1, 1, 1};
    out.V = {0, 0, 0, 0, 1.0f/2, 1, 1, 1, 1};

    // grid size: 5 x 5 (x, y)
    out.P.points = {
        {vec3(-1, 4.2f, 0),    vec3(-0.75f, 4.1f, 0),  vec3(0, 2.5f, 0),   vec3(0.75f, 4.1f, 0),   vec3(1, 4.2f, 0)},
        {vec3(-2, 4.5f, 0), vec3(-1, 4.0f, 0.25f), vec3(0, 2.5f, 0),   vec3(1, 4.0f, 0.25f),  vec3(2, 4.5f, 0)},
        {vec3(-3, 4, 0),    vec3(-1.5f, 3.5f, 1),   vec3(0, 2.5f, 0),   vec3(1.5f, 3.5f, 1),    vec3(3, 4, 0)},
        {vec3(-2, 2, 0),    vec3(-1, 2, 0.5f),      vec3(0, 2, 0.25f),  vec3(1, 2, 0.5f),       vec3(2, 2, 0)},
        {vec3(0, 0, 0),     vec3(0, 0, 0),          vec3(0, 0, 0),      vec3(0, 0, 0),          vec3(0, 0, 0)},
    };

    out.x = 5;
    out.y = 5;

    out.n = out.x - 1;
    out.m = out.y - 1;

    return out;
}

regular_grid nubs::get_grid(uint32_t resolution) const {
    regular_grid out(face_handle(0));
    auto u_max = resolution * x;
    auto v_max = resolution * y;
    out.points.reserve(v_max);

    // TODO: at resolution == 1 this should yield the control polygon. currently there is a small difference - why?
    float current_u = 0;
    float current_v = 0;
    float step_u = static_cast<float>(u_max) / (u_max - 1);
    float step_v = static_cast<float>(v_max) / (v_max - 1);
    for (uint32_t v = 0; v < v_max; ++v) {
        current_u = 0;
        vector<vec3> row;
        row.reserve(u_max);
        for (uint32_t u = 0; u < u_max; ++u) {
            row.push_back(get_surface_point(min(current_u / u_max, 1.0f), min(current_v / v_max, 1.0f)));
            current_u += step_u;
        }
        out.points.push_back(row);
        current_v += step_v;
    }

    return out;
}

vec3 nubs::get_surface_point(float u, float v) const {
    auto u_span = get_u_span(u);
    auto u_basis_funs = get_u_basis_funs(u_span, u);

    auto v_span = get_v_span(v);
    auto v_basis_funs = get_v_basis_funs(v_span, v);

    auto u_ind = u_span - p;
    vec3 out(0, 0, 0);
    for (uint32_t l = 0; l <= q; ++l) {
        vec3 temp(0, 0, 0);
        auto v_ind = v_span - q + l;
        for (uint32_t k = 0; k <= p; ++k) {
            temp += u_basis_funs[k] * P.points[v_ind][u_ind + k];
        }
        out += v_basis_funs[l] * temp;
    }

    return out;
}

size_t nubs::get_u_span(float u) const {
    if (u == U[n + 1]) {
        return n;
    }
    auto low = p;
    auto high = n + 1;
    auto mid = (low + high) / 2;
    while (u < U[mid] || u >= U[mid+1]) {
        if (u < U[mid]) {
            high = mid;
        } else {
            low = mid;
        }
        mid = (low + high) / 2;
    }
    return mid;
}

size_t nubs::get_v_span(float v) const {
    if (v == V[m + 1]) {
        return m;
    }
    auto low = q;
    auto high = m + 1;
    auto mid = (low + high) / 2;
    while (v < V[mid] || v >= V[mid+1]) {
        if (v < V[mid]) {
            high = mid;
        } else {
            low = mid;
        }
        mid = (low + high) / 2;
    }
    return mid;
}

vector<float> nubs::get_u_basis_funs(size_t span, float u) const {
    vector<float> out;
//    out.reserve(p);
    out.push_back(1.0f);
    out.resize(p+1);

    vector<float> left;
    left.reserve(p + 1);
    left.push_back(0.0f);

    vector<float> right;
    right.reserve(p + 1);
    right.push_back(0.0f);

    for (size_t j = 1; j <= p; ++j) {
        left.push_back(u - U[span + 1 - j]);
        right.push_back(U[span + j] - u);

        float saved = 0.0f;
        for (size_t r = 0; r < j; ++r) {
            auto temp = out[r] / (right[r+1]+left[j-r]);
            out[r] = saved + right[r+1] * temp;
            saved = left[j-r] * temp;
        }
        out[j] = saved;
    }

    return out;
}

vector<float> nubs::get_v_basis_funs(size_t span, float v) const {
    vector<float> out;
//    out.reserve(span - q + 1);
    out.push_back(1.0f);
    out.resize(q+1);

    vector<float> left;
    left.reserve(q + 1);
    left.push_back(0.0f);

    vector<float> right;
    right.reserve(q + 1);
    right.push_back(0.0f);

    for (size_t j = 1; j <= q; ++j) {
        left.push_back(v - V[span + 1 - j]);
        right.push_back(V[span + j] - v);

        float saved = 0.0f;
        for (size_t r = 0; r < j; ++r) {
            auto temp = out[r] / (right[r+1]+left[j-r]);
            out[r] = saved + right[r+1] * temp;
            saved = left[j-r] * temp;
        }
        out[j] = saved;
    }

    return out;
}

}
