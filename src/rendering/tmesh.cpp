#include <tsl/rendering/tmesh.hpp>

namespace tsl
{

vector<uint8_t> get_picked_faces_buffer(const vector<regular_grid>& faces, const vector<reference_wrapper<const picking_element>>& picked) {
    // Filter type faces
    vector<reference_wrapper<const picking_element>> faces_picked;
    copy_if(picked.begin(), picked.end(), back_inserter(faces_picked), [](const picking_element& elem) {
        return elem.type == object_type::face;
    });

    size_t num_vertices = 0;
    for (auto&& face: faces) {
        num_vertices += face.num_points_y * face.num_points_x;
    }

    // If no faces are selected, return all zero vec
    if (faces_picked.empty()) {
        return vector<uint8_t>(num_vertices, 0);
    }

    // Transform handles into face handles
    vector<face_handle> picked_handles;
    transform(faces_picked.begin(), faces_picked.end(), back_inserter(picked_handles), [](const picking_element& elem) {
        return face_handle(elem.handle.get_idx());
    });

    // Get selected faces
    vector<uint8_t> out;
    out.reserve(num_vertices);
    for (auto&& face: faces) {
        auto found = find(picked_handles.begin(), picked_handles.end(), face.handle);
        auto picked_val = static_cast<uint8_t>(found != picked_handles.end());
        out.insert(out.end(), face.num_points_x * face.num_points_y, picked_val);
    }

    return out;
}

}
