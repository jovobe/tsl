#include <tsl/Application.hpp>

#include <utility>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <vector>

using std::string;
using std::cout;
using std::endl;
using std::istreambuf_iterator;
using std::bind;
using std::move;
using std::make_pair;
using std::out_of_range;
using std::vector;

namespace tsl {

void Application::init_glfw() const {

    glfwSetErrorCallback(&Application::glfw_error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
}

void Application::glfw_error_callback(int error, const char* description) {
    cout << "ERROR: " << description << endl;
}

void Application::glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    cout << "INFO: key pressed!" << endl;

    auto& instance = getInstance();
    try {
        auto& window = instance.windows.at(glfw_window);
        window.glfw_key_callback(key, scancode, action, mods);
    } catch (out_of_range&) {
        cout << "WARNING: key caught for unknown window!" << endl;
    }
}

void Application::glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    cout << "framebuffer size callback!" << endl;
    // TODO: make this window specific
    glViewport(0, 0, width, height);
}

Application& Application::getInstance() {
    static Application instance;
    return instance;
}

Application::Application() {
    init_glfw();
}

void Application::create_window(string title, uint32_t width, uint32_t height) {
    Window win(move(title), width, height);
    windows.insert(make_pair(win.glfw_window, move(win)));
}

void Application::run() {
    vector<GLFWwindow*> to_close;
    while (true) {
        // TODO: quit the app if quit was pressed, not when no windows are left
        if (windows.empty()) {
            break;
        }

        // render all windows
        for (auto& [k, w] : windows) {
            if (w.should_close()) {
                to_close.push_back(k);
            }

            w.render();
        }

        // close requested windows
        if (!to_close.empty()) {
            for (auto& w: to_close) {
                windows.erase(w);
            }
            to_close.clear();
        }
    }
}

Application::~Application() {
    glfwTerminate();
}

}
