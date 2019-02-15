add_executable(tsl_tests
    ../main.cpp
    geometry/half_edge_mesh_tests.cpp
    attrmaps/stable_vector_tests.cpp
)

target_link_libraries(tsl_tests
    ${CONAN_LIBS}
    tsl
)
