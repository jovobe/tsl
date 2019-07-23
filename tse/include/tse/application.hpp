#ifndef TSE_APPLICATION_HPP
#define TSE_APPLICATION_HPP

#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tse/window.hpp"

using std::string;
using std::map;

namespace tse {

class window;

/**
 * @brief This class represents the running application. Because only one istance of this can be used at a time
 * it is implemented as a singleton.
 */
class application {
public:
    // This should neither be movable nor copyable
    application(application&&) = delete;
    application(application const &) = delete;
    application& operator=(const application&) = delete;
    application& operator=(application&&) = delete;

    ~application();

    /**
     * @brief Returns the application object.
     */
    static application& get_instance();

    /**
     * @brief Returns the number of frames which were rendered in the last second.
     */
    uint32_t get_last_num_frames() const;

    /**
     * @brief Returns the last number of ms the application slept while waiting for the next second to start.
     */
    uint32_t get_last_sleep() const;

    /**
     * @brief Creates a window for the application.
     */
    void create_window(string&& title, uint32_t width, uint32_t height);

    /**
     * @brief Runs the main loop for the application.
     */
    void run();

    // GLFW wrapper functions

    /**
     * @brief Returns the current time.
     */
    double get_time() const;

private:
    /// All windows of the application.
    map<GLFWwindow*, window> windows;

    /// The number of frames which were rendered in the last second.
    uint32_t last_num_frames;

    /// The last number of ms the application slept while waiting for the next second to start.
    uint32_t last_sleep;

    /// The number of frames which should be drawn per second.
    const float FPS_TARGET = 60;

    /// Private ctor because of singleton implementation.
    application();

    /// Initializes GLFW.
    void init_glfw() const;

    // GLFW callbacks
    static void glfw_error_callback(int error, const char* description);
    static void glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
    static void glfw_framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height);
    static void glfw_window_size_callback(GLFWwindow* glfw_window, int width, int height);
    static void glfw_mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods);

    friend class window;
};

}

#endif //TSE_APPLICATION_HPP
