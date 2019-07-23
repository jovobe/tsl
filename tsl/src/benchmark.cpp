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

string duration_to_str(const time_point<high_resolution_clock>& t1, const time_point<high_resolution_clock>& t2, double runs) {
    auto duration_ms = duration_cast<milliseconds>(t2 - t1).count() / runs;
    auto out = format("{}ms", duration_ms);

    if (duration_ms < 100) {
        auto duration_us = duration_cast<microseconds>(t2 - t1).count() / runs;
        out = format("{}μs", duration_us);
    }

    return out;
}

void bench_load_mesh(const string& path, double runs) {
    auto t1 = high_resolution_clock::now();
    for (uint32_t i = 0; i < runs; ++i) {
        read_obj_into_tmesh(path);
    }
    auto t2 = high_resolution_clock::now();
    println("Benchmark load mesh with {} in {} runs avg.: {}", path, runs, duration_to_str(t1, t2, runs));
}

void bench_build_caches(const string& path, double runs) {
    auto mesh = read_obj_into_tmesh(path);
    auto t1 = high_resolution_clock::now();
    for (uint32_t i = 0; i < runs; ++i) {
        auto cloned = mesh;
        surface_evaluator(move(cloned));
    }
    auto t2 = high_resolution_clock::now();
    println("Benchmark build caches with {} in {} runs avg.: {}", path, runs, duration_to_str(t1, t2, runs));
}

void bench_eval_surface(const string& path, double runs, const vector<uint32_t>& resolutions) {
    auto mesh = read_obj_into_tmesh(path);
    auto evaluator = surface_evaluator(move(mesh));
    for (const auto& res: resolutions) {
        auto t1 = high_resolution_clock::now();
        for (uint32_t i = 0; i < runs; ++i) {
            evaluator.eval_per_face(res);
        }
        auto t2 = high_resolution_clock::now();
        println("Benchmark eval surface with {} (resolution: {}) in {} runs avg.: {}", path, res, runs, duration_to_str(t1, t2, runs));
    }
}

/**
 * @brief Runs benchmarks for the three main steps of point calculation.
 *
 * Step 1: Load data into the T-Mesh.
 * Step 2: Build caches of `surface_evaluator`.
 * Step 3: Evaluate points in the surface.
 */
int main(int argc, char* argv[]) {
    if (argc < 4) {
        println("Usage: tsl_benchmark path_to_mesh runs benchmark [resolution]\n");
        println("path_to_mesh: The file path to the mesh to run the benchmark on.");
        println("runs: Number of runs to get the avg. runtime from.");
        println("benchmark: Benchmark to run. This has to be one of: load, build, eval.");
        println("resolution: Resolution for eval benchmark.");
        exit(EXIT_SUCCESS);
    }

    vector<string> args(argv, argv + argc);
    auto filename = args[1];
    auto runs = std::stod(args[2]);
    auto bench = args[3];

    if (bench == "load") {
        bench_load_mesh(filename, runs);
    } else if (bench == "build") {
        bench_build_caches(filename, runs);
    } else if (bench == "eval") {
        auto res = static_cast<uint32_t>(std::stoi(args[4]));
        bench_eval_surface(filename, runs, {res});
    } else {
        println("Unknown benchmark!");
    }

    exit(EXIT_SUCCESS);
}
