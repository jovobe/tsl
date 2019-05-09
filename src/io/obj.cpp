#include <tsl/io/obj.hpp>
#include <tsl/geometry/vector.hpp>

#define TINYOBJLOADER_USE_DOUBLE
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace tsl {

half_edge_mesh read_obj_into_hem(const string& file_path) {

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path.c_str(), nullptr, false);
    if (!ret) {
        if (!err.empty()) {
            panic(err);
        } else {
            panic(warn);
        }
    }

    // Get mesh
    if (shapes.empty()) {
        panic("No shape found inside this .obj!");
    } else if (shapes.size() > 1) {
        panic("The .obj file containes more than one shape!");
    }

    auto mesh = shapes.front();
    half_edge_mesh out;
    vector<vertex_handle> vertex_handles;
    vertex_handles.reserve(attrib.vertices.size() / 3);
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        vertex_handles.push_back(out.add_vertex(vec3(
            attrib.vertices[i + 0],
            attrib.vertices[i + 1],
            attrib.vertices[i + 2]
        )));
    }

    // Loop over faces
    vector<vertex_handle> vertices;
    size_t index_offset = 0;
    for (auto&& fv: mesh.mesh.num_face_vertices) {
        vertices.clear();

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
            // access to vertex
            tinyobj::index_t idx = mesh.mesh.indices[index_offset + v];
            vertices.push_back(vertex_handles[idx.vertex_index]);
        }
        index_offset += fv;

        out.add_face(vertices);
    }

    return out;
}

}
