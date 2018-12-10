#include <tsl/Application.hpp>

#include <utility>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>

using std::string;
using std::cout;
using std::endl;
using std::istreambuf_iterator;
using std::bind;
using std::move;
using std::make_pair;
using std::out_of_range;
using std::vector;
using std::this_thread::sleep_until;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::steady_clock;

namespace tsl {

void Application::init_glfw() const {

    glfwSetErrorCallback(&Application::glfw_error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwSetTime(0);
}

void Application::glfw_error_callback(int error, const char* description) {
    cout << "ERROR: " << description << endl;
}

void Application::glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
//    cout << "INFO: key pressed!" << endl;

    auto& instance = getInstance();
    try {
        auto& window = instance.windows.at(glfw_window);
        window.glfw_key_callback(key, scancode, action, mods);
    } catch (out_of_range&) {
        cout << "WARNING: key caught for unknown window!" << endl;
    }
}

void Application::glfw_framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height) {
    cout << "INFO: framebuffer size callback! (" << width << "," << height << ")" << endl;
    // TODO: make this window specific
    glViewport(0, 0, width, height);
}

void Application::glfw_window_size_callback(GLFWwindow* glfw_window, int width, int height) {
    cout << "INFO: window size callback! (" << width << "," << height << ")" << endl;

    auto& instance = getInstance();
    try {
        auto& window = instance.windows.at(glfw_window);
        window.glfw_window_size_callback(width, height);
    } catch (out_of_range&) {
        cout << "WARNING: window size callback for unknown window!" << endl;
    }
}

void Application::glfw_mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods) {
//    cout << "INFO: mouse button pressed" << endl;

    auto& instance = getInstance();
    try {
        auto& window = instance.windows.at(glfw_window);
        window.glfw_mouse_button_callback(button, action, mods);
    } catch (out_of_range&) {
        cout << "WARNING: window size callback for unknown window!" << endl;
    }
}

void Application::glfw_cursor_pos_callback(GLFWwindow* glfw_window, double x_pos, double y_pos) {
//    cout << "INFO: cursor pos callback! (" << x_pos << "," << y_pos << ")" << endl;

    auto& instance = getInstance();
    try {
        auto& window = instance.windows.at(glfw_window);
        window.glfw_cursor_pos_callback(x_pos, y_pos);
    } catch (out_of_range&) {
        cout << "WARNING: window size callback for unknown window!" << endl;
    }
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
    auto last_time = get_time();
    auto start = steady_clock::now();
    uint32_t num_frames = 0;

    while (true) {
        auto now = steady_clock::now();
        auto diff = now - start;
        auto end = now + milliseconds(static_cast<uint32_t>((1.0f / FPS_TARGET) * 1000));

        // mesaure speed
        auto current_time = get_time();
        num_frames += 1;
        if (current_time - last_time >= 1.0) {
            auto ms_per_frame = 1000.0f / num_frames;
            cout << ms_per_frame << " ms/frame" << endl;
            num_frames = 0;
            last_time += 1.0;
        }

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

        sleep_until(end);
    }
}

Application::~Application() {
    glfwTerminate();
}

double Application::get_time() const {
    return glfwGetTime();
}

}
