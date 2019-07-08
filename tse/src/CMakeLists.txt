add_executable(tse
    main.cpp
    application.cpp
    window.cpp
    read_file.cpp
    camera.cpp
    rendering/grid.cpp
    gl_buffer.cpp
    opengl.cpp
    rendering/picking_map.cpp
    rendering/tmesh.cpp
)

target_include_directories(tse PRIVATE
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(tse
    ${CONAN_LIBS}
    ${OPENGL_LIBRARIES}
    imgui
    tsl
)
