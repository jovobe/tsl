#ifndef TSL_APPLICATION_HPP
#define TSL_APPLICATION_HPP

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <tsl/Window.hpp>

#include <map>

using std::string;
using std::map;

namespace tsl {

class Window;

class Application {
public:
    Application(Application&&) = delete;
    Application(Application const &) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    ~Application();

    static Application& getInstance();

    void create_window(string title, uint32_t width, uint32_t height);
    void run();

private:
    map<GLFWwindow*, Window> windows;

    Application();

    void init_glfw();
    static void glfw_error_callback(int error, const char* description);
    static void glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
    static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);

    // TODO: restrict scope
    friend class Window;
//    friend Window::Window(string title, uint32_t width, uint32_t height);
};

}

#endif //TSL_APPLICATION_HPP
