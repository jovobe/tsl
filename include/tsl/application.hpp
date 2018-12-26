#ifndef TSL_APPLICATION_HPP
#define TSL_APPLICATION_HPP

#include <GLFW/glfw3.h>

#include <tsl/window.hpp>

#include <map>

using std::string;
using std::map;

namespace tsl {

class window;

class application {
public:
    application(application&&) = delete;
    application(application const &) = delete;
    application& operator=(const application&) = delete;
    application& operator=(application&&) = delete;

    ~application();

    static application& get_instance();

    uint32_t get_last_num_frames() const;
    uint32_t get_last_sleep() const;

    void create_window(string title, uint32_t width, uint32_t height);
    void run();

    // GLFW wrapper functions
    double get_time() const;

private:
    map<GLFWwindow*, window> windows;
    uint32_t last_num_frames;
    uint32_t last_sleep;

    const float FPS_TARGET = 60;

    application();

    void init_glfw() const;

    // GLFW callbacks
    static void glfw_error_callback(int error, const char* description);
    static void glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
    static void glfw_framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height);
    static void glfw_window_size_callback(GLFWwindow* glfw_window, int width, int height);
    static void glfw_mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods);

    // TODO: restrict scope
    friend class window;
//    friend window::window(string title, uint32_t width, uint32_t height);
};

}

#endif //TSL_APPLICATION_HPP
