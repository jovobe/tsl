add_executable(tsl_main main.cpp Application.cpp Window.cpp read_file.cpp)

target_include_directories(tsl_main PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(tsl_main ${OPENGL_gl_LIBRARY} glfw glm)
