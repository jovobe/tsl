#ifndef TSL_WINDOW_HPP
#define TSL_WINDOW_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <functional>
#include <set>

#include <tsl/mouse_pos.hpp>
#include <tsl/application.hpp>
#include <tsl/camera.hpp>
#include <tsl/gl_buffer.hpp>
#include <tsl/resolution.hpp>
#include <tsl/tsplines.hpp>
#include <tsl/rendering/picking_map.hpp>

using std::string;
using std::reference_wrapper;
using std::set;

namespace tsl {

struct request_pick_data {
    mouse_pos pos;
    bool single_select;

    request_pick_data(const mouse_pos& pos, bool single_select) : pos(pos), single_select(single_select) {}
};

struct move_direction_t {
    bool x;
    bool y;
    bool z;

    move_direction_t() : x(false), y(false), z(false) {}
};

struct show_dialogs {
    bool settings;
    bool selected_elements;
    bool remove_edges;

    show_dialogs() : settings(false), selected_elements(false), remove_edges(false) {}
};

class window {
public:
    // TODO: make private?
    window(string title, uint32_t width, uint32_t height);
    window(window const &) = delete;
    window(window&& window) noexcept;
    window& operator=(const window&) = delete;
    window& operator=(window&& window) noexcept;

    ~window();

    void glfw_key_callback(int key, int scancode, int action, int mods);
    void glfw_framebuffer_size_callback(int width, int height);
    void glfw_window_size_callback(int width, int height);
    void glfw_mouse_button_callback(int button, int action, int mods);

    void render();

    // GLFW wrapper functions
    bool should_close() const;
    mouse_pos get_mouse_pos() const;

private:
    string title;
    uint32_t width;
    uint32_t height;

    GLsizei frame_width;
    GLsizei frame_height;

    double dpi;

    bool wireframe_mode;
    bool control_mode;
    bool surface_mode;
    bool normal_mode;

    bool move_object;

    show_dialogs dialogs;

    gl_multi_buffer surface_buffer;
    gl_buffer control_edges_buffer;
    gl_buffer control_vertices_buffer;

    // TODO: wrap this in a smart pointer
    // Pointer to glfw window. If this points to nullptr, the window was moved.
    GLFWwindow* glfw_window;

    // picking stuff
    class picking_map picking_map;
    GLuint vertex_picking_program;
    GLuint edge_picking_program;
    GLuint surface_picking_program;

    GLuint control_picking_edges_vertex_array;
    GLuint control_picking_vertices_vertex_array;
    GLuint surface_picking_vertex_array;

    GLuint picking_frame;
    GLuint picking_texture;
    GLuint picking_render;

    optional<vec3> start_move;
    optional<request_pick_data> request_pick;
    set<picking_element> picked_elements;
    optional<picking_element> request_remove;
    move_direction_t move_direction;

    // programs
    GLuint vertex_program;
    GLuint edge_program;
    GLuint phong_program;
    GLuint normal_program;

    // vao
    GLuint surface_vertex_array;
    GLuint surface_normal_vertex_array;
    GLuint control_edges_vertex_array;
    GLuint control_vertices_vertex_array;

    // vbo
    GLuint surface_picked_buffer;
    GLuint edges_picked_buffer;
    GLuint vertices_picked_buffer;
    GLuint surface_vertex_buffer;
    GLuint surface_index_buffer;
    GLuint control_edges_vertex_buffer;
    GLuint control_edges_index_buffer;
    GLuint control_vertices_vertex_buffer;
    GLuint control_vertices_index_buffer;

    resolution<uint32_t> surface_resolution;
    struct tmesh tmesh;

    vector<regular_grid> tmesh_faces;

    class camera camera;

    // TODO: restrict scope
//    friend void application::create_window(string title, uint32_t width, uint32_t height);
    friend class application;

    void update_buffer();

    void update_surface_buffer();
    void update_control_buffer();

    void update_picked_buffer();

    void update_texture_sizes() const;

    void draw_surface(const mat4& model, const mat4& vp) const;
    void draw_surface_normals(const mat4& model, const mat4& vp) const;
    void draw_surface_picking(const mat4& model, const mat4& vp) const;
    void draw_control_polygon(const mat4& model, const mat4& vp) const;
    void draw_control_polygon_picking(const mat4& model, const mat4& vp) const;
    void draw_gui();

    void handle_object_move(const mat4& model, const mat4& vp);
    vec3 get_ray(const mouse_pos& mouse_pos, const mat4& vp) const;

    uint32_t read_pixel(const mouse_pos& pos) const;

    void picking_phase(const mat4& model, const mat4& vp);

    void open_file_dialog_and_load_selected_file();
};

}

#endif //TSL_WINDOW_HPP
