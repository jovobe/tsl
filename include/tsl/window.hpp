#ifndef TSL_WINDOW_HPP
#define TSL_WINDOW_HPP

#include <string>
#include <functional>
#include <set>
#include <memory>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "tsl/mouse_pos.hpp"
#include "tsl/application.hpp"
#include "tsl/camera.hpp"
#include "tsl/gl_buffer.hpp"
#include "tsl/resolution.hpp"
#include "tsl/rendering/picking_map.hpp"
#include "tsl/evaluation/surface_evaluator.hpp"
#include "grid.hpp"

using std::string;
using std::reference_wrapper;
using std::set;
using std::unique_ptr;

namespace tsl {

/**
 * @brief POD type to represent the request to select an element in the scene.
 */
struct request_pick_data {
    /// Mouse position, where the pick was requested.
    mouse_pos pos;
    /// True, if only this element should be selected, false, if this element should be added to the currently
    /// selected elements.
    bool single_select;

    request_pick_data(const mouse_pos& pos, bool single_select) : pos(pos), single_select(single_select) {}
};

/**
 * @brief POD type to represent the direction along which should be moved.
 */
struct move_direction_t {
    bool x;
    bool y;
    bool z;

    move_direction_t() : x(false), y(false), z(false) {}
};

/**
 * @brief POD type to hold information about which dialog shoule be shown in the GUI.
 */
struct show_dialogs {
    /// True, if the settings dialog should be shown, false otherwise.
    bool settings;

    /// True, if the selected elements dialog should be shown, false otherwise.
    bool selected_elements;

    /// True, if the remove edges dialog should be shown, false otherwise.
    bool remove_edges;

    show_dialogs() : settings(false), selected_elements(false), remove_edges(false) {}
};

struct glfw_window_destructor {
    void operator()(GLFWwindow* glfw_window) {
        glfwDestroyWindow(glfw_window);
    }
};
/// A smart pointer to a GLFWwindow.
using glfw_window_ptr = unique_ptr<GLFWwindow, glfw_window_destructor>;

/**
 * @brief This class represents a window in the application.
 */
class window {
public:
    window(string&& title, uint32_t width, uint32_t height);
    window(window const &) = delete;
    window(window&& window) = default;
    window& operator=(const window&) = delete;
    window& operator=(window&& window) = default;

    ~window();

    /**
     * @brief This is called, when a key is pressed inside the window.
     */
    void glfw_key_callback(int key, int scancode, int action, int mods);

    /**
     * @brief This is called when the framebuffer size of the window changes.
     */
    void glfw_framebuffer_size_callback(int width, int height);

    /**
     * @brief This is called, when the window size changes.
     */
    void glfw_window_size_callback(int width, int height);

    /**
     * @brief This is called, when a mouse button is pressed in the window.
     */
    void glfw_mouse_button_callback(int button, int action, int mods);

    /**
     * @brief Renders the current window.
     */
    void render();

    // GLFW wrapper functions

    /**
     * @brief Returns true, when the window should be closed.
     */
    bool should_close() const;

    /**
     * @brief Returns the mouse position in the current window.
     */
    mouse_pos get_mouse_pos() const;

private:
    /// Title of the window.
    string title;
    /// Width of the window.
    uint32_t width;
    /// Height of the window.
    uint32_t height;

    /// Width of the framebuffer of the window.
    GLsizei frame_width;
    /// Height of the framebuffer of the window.
    GLsizei frame_height;

    /// True, if the wireframe of the surface should be shown, false otherwise.
    bool wireframe_mode;
    /// True, if the control polygon of the surface should be shown, false otherwise.
    bool control_mode;
    /// True, if the surface should be shown, false otherwise.
    bool surface_mode;
    /// True, if the normals of the surface should be shown, false otherwise.
    bool normal_mode;
    /// True, if the reflection lines of the surface should be shown, false otherwise.
    bool show_reflection_lines;

    /// True, if the currently selected object should be moved.
    bool move_object;

    /// Collection of bools, which represent, which dialogs should be shown.
    show_dialogs dialogs;

    /// Buffer for the surface.
    gl_multi_buffer surface_buffer;
    /// Buffer for the edges in the control polygon.
    gl_buffer control_edges_buffer;
    /// Buffer for the vertices in the control polygon.
    gl_buffer control_vertices_buffer;

    /// Pointer to glfw window. If this points to nullptr, the window was moved.
    glfw_window_ptr glfw_window;

    // picking stuff
    /// Picking map-
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

    /// If this is not none, it contains the start position of the currently moving object.
    optional<vec3> start_move;
    /// If this is not none, it contains the currently requested element pick.
    optional<request_pick_data> request_pick;
    /// Set of currently picked elements.
    set<picking_element> picked_elements;
    /// If this is not none, it contains the elements which should be removed.
    optional<picking_element> request_remove;
    /// The current direction along wich the currently moved object should be moved.
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

    /// Percentage of edges which should be removed.
    float edge_remove_percentage;
    /// Resolution of the surface.
    resolution<uint32_t> surface_resolution;
    /// Size of cube loaded at start
    int cube_size;
    /// Surface evaluator.
    surface_evaluator evaluator;

    /// Evaluated grids of the surface.
    vector<regular_grid> tmesh_faces;

    /// Camera
    class camera camera;

    friend class application;

    /**
     * @brief Updates all buffers and resets picked elements and picking map.
     */
    void update_buffer();

    /**
     * @brief Updates the surface buffer.
     */
    void update_surface_buffer();

    /**
     * @brief Updates the control polygon buffers.
     */
    void update_control_buffer();

    /**
     * @brief Updates the picking buffer.
     */
    void update_picked_buffer();

    /**
     * @brief Updates the sizes of the used textures.
     */
    void update_texture_sizes() const;

    /**
     * @brief Draws the surface.
     */
    void draw_surface(const mat4& model, const mat4& vp) const;

    /**
     * @brief Draws the surface normals.
     */
    void draw_surface_normals(const mat4& model, const mat4& vp) const;

    /**
     * @brief Draws the surface into the offscreen framebuffer used for picking.
     */
    void draw_surface_picking(const mat4& model, const mat4& vp) const;

    /**
     * @brief Draws the control polygon.
     */
    void draw_control_polygon(const mat4& model, const mat4& vp) const;

    /**
     * @brief Draws the control polygon into the offscreen framebuffer used for picking.
     */
    void draw_control_polygon_picking(const mat4& model, const mat4& vp) const;

    /**
     * @brief Draws the GUI.
     */
    void draw_gui();

    /**
     * @brief Handles the movement of the selected objects.
     */
    void handle_object_move(const mat4& model, const mat4& vp);

    /**
     * @brief Creates a ray, which points from the given mouse pos to the far plane.
     */
    vec3 get_ray(const mouse_pos& mouse_pos, const mat4& vp) const;

    /**
     * @brief Reads the pixel at the given position.
     */
    uint32_t read_pixel(const mouse_pos& pos) const;

    /**
     * @brief Handles the picking request.
     */
    void picking_phase(const mat4& model, const mat4& vp);

    /**
     * @brief Opens a file selection dialog and loads the selected quadmesh into a tmesh.
     */
    void open_file_dialog_and_load_selected_file();
};

}

#endif //TSL_WINDOW_HPP
