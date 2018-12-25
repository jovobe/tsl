#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
    wireframe_mode(false),
    control_mode(true),
    surface_mode(true),
    nubs_resolution()
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

    // GLEW
    if (glewInit() != GLEW_OK) {
        cout << "ERROR: Failed to init GLEW!" << endl;
        exit(EXIT_FAILURE);
    }

    // ImGui
    const char* glsl_version = "#version 330";
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

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
//    surface_buffer.vertex_buffer = {
//        vec3(0.5f, 0.5f, 0.0f),
//        vec3(0.5f, -0.5f, 0.0f),
//        vec3(-0.5f, -0.5f, 0.0f),
//        vec3(-0.5f, 0.5f, 0.0f)
//    };
//    surface_buffer.index_buffer = {
//        0, 1, 3,
//        1, 2, 3
//    };

    nubs = get_example_data_1();
//    auto data = get_example_data_2();
//    auto data = get_example_data_3();
//    auto data = get_example_data_4();

    // surface
    glGenVertexArrays(1, &surface_vertex_array);
    glGenBuffers(1, &surface_vertex_buffer);
    glGenBuffers(1, &surface_index_buffer);

    update_nubs_buffer();

    // control polygon
    glGenVertexArrays(1, &control_vertex_array);
    glGenBuffers(1, &control_vertex_buffer);
    glGenBuffers(1, &control_index_buffer);
    glBindVertexArray(control_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, control_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, control_buffer.vertex_buffer.size() * sizeof(vec3), control_buffer.vertex_buffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, control_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, control_buffer.index_buffer.size() * sizeof(uint32_t), control_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // pointer binding
    auto control_vpos_location = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(static_cast<GLuint>(control_vpos_location), 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(static_cast<GLuint>(control_vpos_location));

//    auto vcolor_location = glGetAttribLocation(program, "color_in");
//    glVertexAttribPointer(static_cast<GLuint>(vcolor_location), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (sizeof(float) * 3));
//    glEnableVertexAttribArray(static_cast<GLuint>(vcolor_location));
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
                    } else {
                        camera.moving_direction.forward = true;
                    }
                    break;
                case GLFW_KEY_S:
                    if ((mods & GLFW_MOD_CONTROL) != 0) {
                        cout << "INFO: toggled surface mode" << endl;
                        surface_mode = !surface_mode;
                    } else {
                        camera.moving_direction.backwards = true;
                    }
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
                    if ((mods & GLFW_MOD_CONTROL) != 0) {
                        cout << "INFO: toggled control plygon mode" << endl;
                        control_mode = !control_mode;
                    } else {
                        camera.moving_direction.down = true;
                    }
                    break;
                case GLFW_KEY_R:
                    camera.reset_position();
                    break;
                // TODO: switch to keyboard layout independent version! (use `glfwSetCharCallback`)
                case GLFW_KEY_RIGHT_BRACKET:
                    nubs_resolution.increment();
                    update_nubs_buffer();
                    break;
                case GLFW_KEY_SLASH:
                    nubs_resolution.decrement();
                    update_nubs_buffer();
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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Settings");
        ImGui::Checkbox("Wireframe mode", &wireframe_mode);
        ImGui::Checkbox("Show control polygon", &control_mode);
        ImGui::Checkbox("Show surface", &surface_mode);
        static int slider_resolution=1;
        if (ImGui::SliderInt("Resolution", &slider_resolution, 1, 40)) {
            nubs_resolution.set(static_cast<uint32_t>(slider_resolution));
            update_nubs_buffer();
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::Render();

    glfwMakeContextCurrent(glfw_window);
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

    auto color_location = glGetUniformLocation(program, "color_in");
    glUniformMatrix4fv(color_location, 1, GL_FALSE, value_ptr(mvp));
    glUniform3fv(color_location, 1, value_ptr(vec3(0, 1, 0)));

    if (surface_mode) {
        glBindVertexArray(surface_vertex_array);
        if (wireframe_mode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(surface_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));
    }

    if (control_mode) {
        glUniform3fv(color_location, 1, value_ptr(vec3(1, 0, 0)));
        glBindVertexArray(control_vertex_array);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(control_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(glfw_window);
    glfwSwapBuffers(glfw_window);
    glfwPollEvents();
}

window::~window() {
    // if this window was moved, we don't have to destruct it
    if (glfw_window != nullptr) {
        // TODO: add checks if array and buffers need to be deleted
        glDeleteVertexArrays(1, &surface_vertex_array);
        glDeleteBuffers(1, &surface_vertex_buffer);
        glDeleteBuffers(1, &surface_index_buffer);

        glDeleteVertexArrays(1, &control_vertex_array);
        glDeleteBuffers(1, &control_vertex_buffer);
        glDeleteBuffers(1, &control_index_buffer);

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

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
    surface_vertex_array = exchange(window.surface_vertex_array, 0);
    surface_vertex_buffer = exchange(window.surface_vertex_buffer, 0);
    surface_index_buffer = exchange(window.surface_index_buffer, 0);
    control_vertex_array = exchange(window.control_vertex_array, 0);
    control_vertex_buffer = exchange(window.control_vertex_buffer, 0);
    control_index_buffer = exchange(window.control_index_buffer, 0);
    wireframe_mode = exchange(window.wireframe_mode, false);
    control_mode = exchange(window.control_mode, false);
    surface_mode = exchange(window.surface_mode, false);
    surface_buffer = move(window.surface_buffer);
    control_buffer = move(window.control_buffer);
    nubs = move(window.nubs);
    nubs_resolution = move(window.nubs_resolution);

    return *this;
}

mouse_pos window::get_mouse_pos() const {
    double x_pos;
    double y_pos;
    glfwGetCursorPos(glfw_window, &x_pos, &y_pos);
    return mouse_pos(x_pos, y_pos);
}

void window::load_nubs_data_to_gpu() const {
    glBindVertexArray(surface_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, surface_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, surface_buffer.vertex_buffer.size() * sizeof(vec3), surface_buffer.vertex_buffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surface_buffer.index_buffer.size() * sizeof(uint32_t), surface_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // TODO: perhaps this is not necessary?
    // pointer binding
    auto vpos_location = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(static_cast<GLuint>(vpos_location), 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(static_cast<GLuint>(vpos_location));

//    auto vcolor_location = glGetAttribLocation(program, "color_in");
//    glVertexAttribPointer(static_cast<GLuint>(vcolor_location), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (sizeof(float) * 3));
//    glEnableVertexAttribArray(static_cast<GLuint>(vcolor_location));
}

void window::update_nubs_buffer() {
    auto grid = nubs.get_grid(nubs_resolution.get());
    surface_buffer = grid.get_render_buffer();
    control_buffer = nubs.P.get_render_buffer();

    load_nubs_data_to_gpu();
}

}
