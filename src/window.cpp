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
#include <tsl/opengl.hpp>
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
    nubs_resolution(1),
    slider_resolution(1)
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

    program = create_program("shader/vertex.glsl", "shader/fragment.glsl");
    phong_program = create_program("shader/phong/vertex.glsl", "shader/phong/fragment.glsl");

    nubs = nubs::get_example_data_1();
//    auto data = nubs::get_example_data_2();
//    auto data = nubs::get_example_data_3();
//    auto data = nubs::get_example_data_4();

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

    glEnable(GL_DEPTH_TEST);
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
                    slider_resolution += 1;
                    update_nubs_buffer();
                    break;
                case GLFW_KEY_SLASH:
                    nubs_resolution.decrement();
                    if (slider_resolution > 1) {
                        slider_resolution -= 1;
                    }
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
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);

        ImGui::Begin("Settings");
        ImGui::Checkbox("Wireframe mode", &wireframe_mode);
        ImGui::Checkbox("Show control polygon", &control_mode);
        ImGui::Checkbox("Show surface", &surface_mode);

        if (ImGui::SliderInt("Resolution", &slider_resolution, 1, 40)) {
            nubs_resolution.set(static_cast<uint32_t>(slider_resolution));
            update_nubs_buffer();
        }

        auto& app = application::get_instance();
        auto last_num_frames = app.get_last_num_frames();
        if (last_num_frames > 0) {
            auto ms_per_frame = 1000.0f / last_num_frames;
            auto sleep_per_frame = app.get_last_sleep() / last_num_frames;
            ImGui::Text("Rendering: %.3f ms/frame (%.1f FPS, sleeping: %d ms)", ms_per_frame, ImGui::GetIO().Framerate, sleep_per_frame);
        } else {
            ImGui::Text("Rendering: / ms/frame (%.1f FPS, sleeping: / ms)", ImGui::GetIO().Framerate);
        }

        ImGui::End();
    }

    ImGui::Render();

    glfwMakeContextCurrent(glfw_window);
    camera.handle_moving_direction(get_mouse_pos());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // projection
    auto projection = perspective(radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
    auto view = camera.get_view_matrix();
    auto model = rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    auto vp = projection * view;

    if (surface_mode) {
        glUseProgram(phong_program);

        auto vp_location_phong = glGetUniformLocation(phong_program, "VP");
        auto m_location_phong = glGetUniformLocation(phong_program, "M");
        auto color_location_phong = glGetUniformLocation(phong_program, "color_in");
        auto camera_location = glGetUniformLocation(phong_program, "camera_pos");

        glUniformMatrix4fv(vp_location_phong, 1, GL_FALSE, value_ptr(vp));
        glUniformMatrix4fv(m_location_phong, 1, GL_FALSE, value_ptr(model));
        glUniform3fv(color_location_phong, 1, value_ptr(vec3(0, 1, 0)));
        glUniform3fv(camera_location, 1, value_ptr(camera.get_pos()));

        glBindVertexArray(surface_vertex_array);
        if (wireframe_mode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(surface_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));
    }

    if (control_mode) {
        glUseProgram(program);

        auto vp_location = glGetUniformLocation(program, "VP");
        auto m_location = glGetUniformLocation(program, "M");
        auto color_location = glGetUniformLocation(program, "color_in");

        glUniformMatrix4fv(vp_location, 1, GL_FALSE, value_ptr(vp));
        glUniformMatrix4fv(m_location, 1, GL_FALSE, value_ptr(model));
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

window::window(window&& window) noexcept : nubs_resolution(1) {
    *this = move(window);
}

window& window::operator=(window&& window) noexcept {
    glfw_window = exchange(window.glfw_window, nullptr);
    title = move(window.title);
    width = exchange(window.width, 0);
    height = exchange(window.height, 0);
    program = exchange(window.program, 0);
    phong_program = exchange(window.phong_program, 0);
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
    slider_resolution = exchange(window.slider_resolution, 0);

    return *this;
}

mouse_pos window::get_mouse_pos() const {
    double x_pos;
    double y_pos;
    glfwGetCursorPos(glfw_window, &x_pos, &y_pos);
    return mouse_pos(x_pos, y_pos);
}

void window::load_nubs_data_to_gpu() const {
    auto vec_data = surface_buffer.get_combined_vec_data();

    glBindVertexArray(surface_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, surface_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vec_data.size() * sizeof(vec3), vec_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surface_buffer.index_buffer.size() * sizeof(uint32_t), surface_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // TODO: perhaps this is not necessary?
    // pointer binding
    auto vpos_location = glGetAttribLocation(phong_program, "pos_in");
    glVertexAttribPointer(static_cast<GLuint>(vpos_location), 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*) 0);
    glEnableVertexAttribArray(static_cast<GLuint>(vpos_location));

    auto vnormal_location = glGetAttribLocation(phong_program, "normal_in");
    glVertexAttribPointer(static_cast<GLuint>(vnormal_location), 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*) (sizeof(vec3)));
    glEnableVertexAttribArray(static_cast<GLuint>(vnormal_location));

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
