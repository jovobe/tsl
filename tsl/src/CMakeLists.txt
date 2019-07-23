# TSL
add_library(tsl SHARED STATIC
    algorithm/generator.cpp
    algorithm/get_vertices.cpp
    algorithm/reduction.cpp
    evaluation/subdevision.cpp
    evaluation/surface_evaluator.cpp
    geometry/line.cpp
    geometry/line_segment.cpp
    geometry/rectangle.cpp
    geometry/tmesh/iterator.cpp
    geometry/tmesh/tmesh.cpp
    geometry/transform.cpp
    io/obj.cpp
)

target_link_libraries(tsl
    PUBLIC fmt
    PUBLIC glm
    PUBLIC blas
)

# TSL Benchmark
add_executable(tsl_benchmark
    benchmark.cpp
)

target_link_libraries(tsl_benchmark
    tsl
)
