#include <tsl/application.hpp>

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
using std::chrono::duration_cast;

namespace tsl {

void application::init_glfw() const {

    glfwSetErrorCallback(&application::glfw_error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwSetTime(0);
}

void application::glfw_error_callback(int error, const char* description) {
    cout << "ERROR: " << description << endl;
}

void application::glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
//    cout << "INFO: key pressed!" << endl;

//    if (action == GLFW_PRESS) {
//        cout << "INFO: key pressed!" << endl;
//    }

    auto& instance = get_instance();
    try {
        auto& window = instance.windows.at(glfw_window);
        window.glfw_key_callback(key, scancode, action, mods);
    } catch (out_of_range&) {
        cout << "WARNING: key caught for unknown window!" << endl;
    }
}

void application::glfw_framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height) {
    cout << "INFO: framebuffer size callback! (" << width << "," << height << ")" << endl;
    // TODO: make this window specific
    glViewport(0, 0, width, height);
}

void application::glfw_window_size_callback(GLFWwindow* glfw_window, int width, int height) {
    cout << "INFO: window size callback! (" << width << "," << height << ")" << endl;

    auto& instance = get_instance();
    try {
        auto& window = instance.windows.at(glfw_window);
        window.glfw_window_size_callback(width, height);
    } catch (out_of_range&) {
        cout << "WARNING: window size callback for unknown window!" << endl;
    }
}

void application::glfw_mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods) {
//    cout << "INFO: mouse button pressed" << endl;

    auto& instance = get_instance();
    try {
        auto& window = instance.windows.at(glfw_window);
        window.glfw_mouse_button_callback(button, action, mods);
    } catch (out_of_range&) {
        cout << "WARNING: window size callback for unknown window!" << endl;
    }
}

application& application::get_instance() {
    static application instance;
    return instance;
}

application::application() {
    init_glfw();
}

void application::create_window(string title, uint32_t width, uint32_t height) {
    window win(move(title), width, height);
    windows.insert(make_pair(win.glfw_window, move(win)));
}

void application::run() {
    vector<GLFWwindow*> to_close;
    auto last_time = get_time();
    auto start = steady_clock::now();
    uint32_t num_frames = 0;
    int32_t num_sleep = 0;
    auto ms = milliseconds(1);

    while (true) {
        auto now = steady_clock::now();
        auto diff = now - start;
        auto end = now + milliseconds(static_cast<uint32_t>((1.0f / FPS_TARGET) * 1000));

        // mesaure speed
        auto current_time = get_time();
        num_frames += 1;
        if (current_time - last_time >= 1.0) {
            auto ms_per_frame = 1000.0f / num_frames;
            auto sleep_per_frame = num_sleep / num_frames;
            cout << ms_per_frame << " ms/frame (sleeping: " << sleep_per_frame << " ms)" << endl;
            num_frames = 0;
            num_sleep = 0;
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

        auto sleep = duration_cast<milliseconds>(end - steady_clock::now()).count();
        if (sleep > 0) {
            num_sleep += sleep;
        }
        sleep_until(end);
    }
}

application::~application() {
    glfwTerminate();
}

double application::get_time() const {
    return glfwGetTime();
}

}