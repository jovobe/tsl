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

add_executable(tsl_main
    main.cpp
    application.cpp
    window.cpp
    read_file.cpp
    camera.cpp
    grid.cpp
    gl_buffer.cpp
    opengl.cpp
    rendering/picking_map.cpp
    rendering/tmesh.cpp
)

target_include_directories(tsl_main PRIVATE
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(tsl_main
    ${CONAN_LIBS}
    ${OPENGL_LIBRARIES}
    imgui
    tsl
)
