#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tsl/window.hpp>
#include <tsl/application.hpp>
#include <tsl/read_file.hpp>
#include <tsl/nubs.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <utility>

using std::move;
using std::exchange;
using std::string;
using std::cout;
using std::endl;
using std::vector;

using glm::radians;
using glm::vec3;
using glm::mat4;
using glm::value_ptr;
using glm::perspective;
using glm::lookAt;
using glm::rotate;

namespace tsl {

window::window(string title, uint32_t width, uint32_t height) :
    title(move(title)),
    width(width),
    height(height),
    camera(),
    wireframe_mode(false)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

//    glfwWindowHint(GLFW_SAMPLES, 0);
//    glfwWindowHint(GLFW_RED_BITS, 8);
//    glfwWindowHint(GLFW_GREEN_BITS, 8);
//    glfwWindowHint(GLFW_BLUE_BITS, 8);
//    glfwWindowHint(GLFW_ALPHA_BITS, 8);
//    glfwWindowHint(GLFW_STENCIL_BITS, 8);
//    glfwWindowHint(GLFW_DEPTH_BITS, 24);
//    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glfw_window = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
    if (glfw_window == nullptr) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfw_window);
    glfwSetKeyCallback(glfw_window, &application::glfw_key_callback);
    glfwSetFramebufferSizeCallback(glfw_window, &application::glfw_framebuffer_size_callback);
    glfwSetWindowSizeCallback(glfw_window, &application::glfw_window_size_callback);
    glfwSetMouseButtonCallback(glfw_window, &application::glfw_mouse_button_callback);
    glfwSwapInterval(1);

    // Vertex shader
    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto vertex_shader_text = read_file("shader/vertex.glsl");
//    cout << vertex_shader_text << endl;
    // TODO: why variable?
    auto tmp1 = vertex_shader_text.c_str();
    glShaderSource(vertex_shader, 1, &tmp1, nullptr);
    glCompileShader(vertex_shader);
    GLint res;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &res);
    cout << "vertex compile status: " << res << endl;
    if (res == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

        vector<GLchar> error_log(static_cast<unsigned long>(max_length));
        glGetShaderInfoLog(vertex_shader, max_length, &max_length, &error_log[0]);
        string log(error_log.begin(), error_log.end());

        // pop null terminator
        log.pop_back();

        cout << "Compiling shader failed:\n" << log << endl;

        glDeleteShader(vertex_shader); // Don't leak the shader.
        exit(1);
    }

    // Fragment shader
    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragment_shader_text = read_file("shader/fragment.glsl");
    tmp1 = fragment_shader_text.c_str();
    glShaderSource(fragment_shader, 1, &tmp1, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &res);
    cout << "fragment compile status: " << res << endl;
    if (res == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

        vector<GLchar> error_log(static_cast<unsigned long>(max_length));
        glGetShaderInfoLog(fragment_shader, max_length, &max_length, &error_log[0]);
        string log(error_log.begin(), error_log.end());

        // pop null terminator
        log.pop_back();

        cout << "Compiling shader failed:\n" << log << endl;

        glDeleteShader(fragment_shader); // Don't leak the shader.
        exit(1);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    cout << "program link status: " << is_linked << endl;
    if (is_linked == GL_FALSE)
    {
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        std::vector<GLchar> info_log(static_cast<unsigned long>(max_length));
        glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        string log(info_log.begin(), info_log.end());

        // pop null terminator
        log.pop_back();

        cout << "Linking program failed:\n" << log << endl;

        // In this simple program, we'll just leave
        exit(1);
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Vertex data
    draw_buffer.vertex_buffer = {
        vec3(0.5f, 0.5f, 0.0f),
        vec3(0.5f, -0.5f, 0.0f),
        vec3(-0.5f, -0.5f, 0.0f),
        vec3(-0.5f, 0.5f, 0.0f)
    };
    draw_buffer.index_buffer = {
        0, 1, 3,
        1, 2, 3
    };

    auto data = get_example_data_1();
//    auto data = get_example_data_2();
//    auto data = get_example_data_3();
//    auto data = get_example_data_4();
    auto grid = data.get_grid(10);
    auto grid_draw_buffer = grid.get_render_buffer();
//    draw_buffer = data.P.get_render_buffer();
    draw_buffer = grid_draw_buffer;

    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &index_buffer);
    glBindVertexArray(vertex_array);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, draw_buffer.vertex_buffer.size() * sizeof(vec3), draw_buffer.vertex_buffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, draw_buffer.index_buffer.size() * sizeof(uint32_t), draw_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // pointer binding
    auto vpos_location = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(static_cast<GLuint>(vpos_location), 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(static_cast<GLuint>(vpos_location));

//    auto vcolor_location = glGetAttribLocation(program, "color_in");
//    glVertexAttribPointer(static_cast<GLuint>(vcolor_location), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (sizeof(float) * 3));
//    glEnableVertexAttribArray(static_cast<GLuint>(vcolor_location));

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void window::glfw_key_callback(int key, int scancode, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
                    break;
                case GLFW_KEY_W:
                    if ((mods & GLFW_MOD_CONTROL) != 0) {
                        cout << "INFO: toggled wireframe mode" << endl;
                        wireframe_mode = !wireframe_mode;
                        glBindVertexArray(vertex_array);
                        if (wireframe_mode) {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                        } else {
                            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                        }
                    } else {
                        camera.moving_direction.forward = true;
                    }
                    break;
                case GLFW_KEY_S:
                    camera.moving_direction.backwards = true;
                    break;
                case GLFW_KEY_A:
                    camera.moving_direction.left = true;
                    break;
                case GLFW_KEY_D:
                    camera.moving_direction.right = true;
                    break;
                case GLFW_KEY_SPACE:
                    camera.moving_direction.up = true;
                    break;
                case GLFW_KEY_C:
                    camera.moving_direction.down = true;
                    break;
                case GLFW_KEY_R:
                    camera.reset_position();
                    break;
                default:
                    break;
            }
            break;
        case GLFW_RELEASE:
            switch (key) {
                // TODO: weird behaviour, when releasing a key while another one is pressed? -> reset to early?
                case GLFW_KEY_W:
                    camera.moving_direction.forward = false;
                    camera.reset_last_move_time();
                    break;
                case GLFW_KEY_S:
                    camera.moving_direction.backwards = false;
                    camera.reset_last_move_time();
                    break;
                case GLFW_KEY_A:
                    camera.moving_direction.left = false;
                    camera.reset_last_move_time();
                    break;
                case GLFW_KEY_D:
                    camera.moving_direction.right = false;
                    camera.reset_last_move_time();
                    break;
                case GLFW_KEY_SPACE:
                    camera.moving_direction.up = false;
                    camera.reset_last_move_time();
                    break;
                case GLFW_KEY_C:
                    camera.moving_direction.down = false;
                    camera.reset_last_move_time();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void window::glfw_framebuffer_size_callback(int width, int height) {
    // TODO: implement
}

void window::glfw_window_size_callback(int width, int height) {
    this->width = static_cast<uint32_t>(width);
    this->height = static_cast<uint32_t>(height);
}

void window::glfw_mouse_button_callback(int button, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            switch (button) {
                case GLFW_MOUSE_BUTTON_RIGHT:
                    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    camera.moving_direction.mouse = true;
                    break;
                default:
                    break;
            }
            break;
        case GLFW_RELEASE:
            switch (button) {
                case GLFW_MOUSE_BUTTON_RIGHT:
                    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    camera.moving_direction.mouse = false;
                    camera.reset_last_move_time();
                    camera.reset_curos_pos();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void window::render() {
    camera.handle_moving_direction(get_mouse_pos());

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    // projection
    auto projection = perspective(radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
    auto view = camera.get_view_matrix();
    auto model = rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    auto mvp = projection * view * model;
    auto mvp_location = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, value_ptr(mvp));

    glBindVertexArray(vertex_array);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(draw_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));

    glfwSwapBuffers(glfw_window);
    glfwPollEvents();
}

window::~window() {
    // if this window was moved, we don't have to destruct it
    if (glfw_window != nullptr) {
        // TODO: add checks if array and buffers need to be deleted
        glDeleteVertexArrays(1, &vertex_array);
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteBuffers(1, &index_buffer);

        glfwDestroyWindow(glfw_window);
    }
}

bool window::should_close() const {
    return (glfwWindowShouldClose(glfw_window) != 0);
}

window::window(window&& window) noexcept {
    *this = move(window);
}

window& window::operator=(window&& window) noexcept {
    glfw_window = exchange(window.glfw_window, nullptr);
    title = move(window.title);
    width = exchange(window.width, 0);
    height = exchange(window.height, 0);
    program = exchange(window.program, 0);
    vertex_array = exchange(window.vertex_array, 0);
    vertex_buffer = exchange(window.vertex_buffer, 0);
    index_buffer = exchange(window.index_buffer, 0);
    wireframe_mode = exchange(window.wireframe_mode, false);
    draw_buffer = move(window.draw_buffer);

    return *this;
}

mouse_pos window::get_mouse_pos() const {
    double x_pos;
    double y_pos;
    glfwGetCursorPos(glfw_window, &x_pos, &y_pos);
    return mouse_pos(x_pos, y_pos);
}

}
