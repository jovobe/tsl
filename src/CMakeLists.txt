add_executable(tsl_main
    main.cpp
    application.cpp
    window.cpp
    read_file.cpp
    camera.cpp
    nubs.cpp
    grid.cpp
    gl_buffer.cpp
    opengl.cpp
)

add_library(tsl SHARED STATIC
    geometry/half_edge_mesh.cpp
    geometry/line.cpp
    geometry/rectangle.cpp
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
