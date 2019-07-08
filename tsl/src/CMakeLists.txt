add_library(tsl SHARED STATIC
    geometry/line_segment.cpp
    geometry/rectangle.cpp
    evaluation/subdevision.cpp
    geometry/line.cpp
    io/obj.cpp
    geometry/tmesh/tmesh.cpp
    algorithm/reduction.cpp
    evaluation/surface_evaluator.cpp
    evaluation/bsplines.cpp
    algorithm/generator.cpp
    geometry/tmesh/iterator.cpp
    geometry/transform.cpp
    algorithm/get_vertices.cpp
)

target_link_libraries(tsl
    ${CONAN_LIBS}
)
