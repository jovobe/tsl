#ifndef TSL_WINDOW_HPP
#define TSL_WINDOW_HPP

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>

#include <tsl/application.hpp>
#include <tsl/camera.hpp>

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
    void glfw_cursor_pos_callback(double x_pos, double y_pos);

    void enable_cursor_pos_callback();
    void disable_cursor_pos_callback();

    void render() const;
    bool should_close() const;

private:
    string title;
    uint32_t width;
    uint32_t height;

    // TODO: wrap this in a smart pointer
    // Pointer to glfw window. If this points to nullptr, the window was moved.
    GLFWwindow* glfw_window;

    GLuint program;
    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint index_buffer;

    camera camera;

    // TODO: restrict scope
//    friend void application::create_window(string title, uint32_t width, uint32_t height);
    friend class application;
};

}

#endif //TSL_WINDOW_HPP
