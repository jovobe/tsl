#include <string>
#include <vector>
#include <chrono>

#include <fmt/format.h>

#include "tsl/evaluation/surface_evaluator.hpp"
#include "tsl/util/println.hpp"
#include "tsl/io/obj.hpp"

using std::string;
using std::vector;
using namespace std::chrono;

using fmt::format;

using namespace tsl;

void bench_load_mesh(const string& filename, double runs) {
    auto path = format("meshes/{}", filename);
    auto t1 = high_resolution_clock::now();
    for (uint32_t i = 0; i < runs; ++i) {
        read_obj_into_tmesh(path);
    }
    auto t2 = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(t2 - t1).count() / runs;
    println("Benchmark load mesh with {} in {} runs avg.: {}ms", filename, runs, duration);
}

void bench_build_caches(const string& filename, double runs) {
    auto path = format("meshes/{}", filename);
    auto mesh = read_obj_into_tmesh(path);
    auto t1 = high_resolution_clock::now();
    for (uint32_t i = 0; i < runs; ++i) {
        auto cloned = mesh;
        surface_evaluator(move(cloned));
    }
    auto t2 = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(t2 - t1).count() / runs;
    println("Benchmark build caches with {} in {} runs avg.: {}ms", filename, runs, duration);
}

void bench_eval_surface(const string& filename, double runs, const vector<uint32_t>& resolutions) {
    auto path = format("meshes/{}", filename);
    auto mesh = read_obj_into_tmesh(path);
    auto evaluator = surface_evaluator(move(mesh));
    for (const auto& res: resolutions) {
        auto t1 = high_resolution_clock::now();
        for (uint32_t i = 0; i < runs; ++i) {
            evaluator.eval_per_face(res);
        }
        auto t2 = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(t2 - t1).count() / runs;
        println("Benchmark eval surface with {} (resolution: {}) in {} runs avg.: {}ms", filename, res, runs, duration);
    }
}

void bench(const string& filename) {
    bench_load_mesh(filename, 100.0);
    bench_build_caches(filename, 100.0);
    bench_eval_surface(filename, 100.0, {1, 2, 3, 4, 5});
}

int main() {
    auto small = "fandisk.obj";
    auto large = "bumpy_sphere.obj";
    bench(small);
    bench(large);
    exit(EXIT_SUCCESS);
}
