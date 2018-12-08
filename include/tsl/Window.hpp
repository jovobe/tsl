#ifndef TSL_WINDOW_HPP
#define TSL_WINDOW_HPP

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>

#include <tsl/Application.hpp>

using std::string;

namespace tsl {

class Window {
public:
    // TODO: make private?
    Window(string title, uint32_t width, uint32_t height);
    Window(Window const &) = delete;
    Window(Window&& window) noexcept;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&& window) noexcept;

    ~Window();

    void glfw_key_callback(int key, int scancode, int action, int mods);
    void glfw_framebuffer_size_callback(int width, int height);

    void render();
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

    // TODO: restrict scope
//    friend void Application::create_window(string title, uint32_t width, uint32_t height);
    friend class Application;
};

}

#endif //TSL_WINDOW_HPP
