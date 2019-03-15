#ifndef TSL_WINDOW_HPP
#define TSL_WINDOW_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include <tsl/mouse_pos.hpp>
#include <tsl/application.hpp>
#include <tsl/camera.hpp>
#include <tsl/gl_buffer.hpp>
#include <tsl/resolution.hpp>
#include <tsl/tsplines.hpp>

using std::string;

namespace tsl {

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

    bool wireframe_mode;
    bool control_mode;
    bool surface_mode;

    gl_buffer surface_buffer;
    gl_buffer control_buffer;

    // TODO: wrap this in a smart pointer
    // Pointer to glfw window. If this points to nullptr, the window was moved.
    GLFWwindow* glfw_window;

    int slider_resolution;

    GLuint vertex_program;
    GLuint polygon_program;
    GLuint phong_program;
    GLuint surface_vertex_array;
    GLuint surface_vertex_buffer;
    GLuint surface_index_buffer;

    GLuint control_vertex_array;
    GLuint control_vertex_buffer;
    GLuint control_index_buffer;

    resolution<uint32_t> surface_resolution;
    struct tmesh tmesh;

    class camera camera;

    // TODO: restrict scope
//    friend void application::create_window(string title, uint32_t width, uint32_t height);
    friend class application;

    void load_surface_data_to_gpu() const;
    void update_surface_buffer();
};

}

#endif //TSL_WINDOW_HPP
