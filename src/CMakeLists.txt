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

target_include_directories(tsl_main PRIVATE
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(tsl_main
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw
    glm
    imgui
)
