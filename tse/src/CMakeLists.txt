add_executable(tse
    main.cpp
    application.cpp
    window.cpp
    read_file.cpp
    camera.cpp
    gl_buffer.cpp
    opengl.cpp
    rendering/picking_map.cpp
    rendering/tmesh.cpp
    rendering/grid.cpp
)

target_include_directories(tse PRIVATE
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(tse
    ${OPENGL_LIBRARIES}
    imgui
    tsl
    glfw
    libglew_static
)
