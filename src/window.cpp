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
#include <tsl/tsplines.hpp>
#include <tsl/rendering/half_edge_mesh.hpp>

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
    surface_resolution(1),
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

    auto vertex_shader = create_shader("shader/vertex.glsl", GL_VERTEX_SHADER);
    auto fragment_shader = create_shader("shader/fragment.glsl", GL_FRAGMENT_SHADER);
    auto polygon_geometry_shader = create_shader("shader/polygon/geometry.glsl", GL_GEOMETRY_SHADER);
    auto vertex_geometry_shader = create_shader("shader/vertex/geometry.glsl", GL_GEOMETRY_SHADER);

    auto phong_vertex_shader = create_shader("shader/phong/vertex.glsl", GL_VERTEX_SHADER);
    auto phong_fragment_shader = create_shader("shader/phong/fragment.glsl", GL_FRAGMENT_SHADER);

    polygon_program = create_program({vertex_shader, fragment_shader, polygon_geometry_shader});
    vertex_program = create_program({vertex_shader, fragment_shader, vertex_geometry_shader});
    phong_program = create_program({phong_vertex_shader, phong_fragment_shader});

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(polygon_geometry_shader);
    glDeleteShader(vertex_geometry_shader);
    glDeleteShader(phong_vertex_shader);
    glDeleteShader(phong_fragment_shader);

//    tmesh = tsplines::get_example_data_1();
    tmesh = tsplines::get_example_data_2(5);

    // surface
    glGenVertexArrays(1, &surface_vertex_array);
    glGenBuffers(1, &surface_vertex_buffer);
    glGenBuffers(1, &surface_index_buffer);

    update_surface_buffer();

    // control polygon
    glGenVertexArrays(1, &control_vertex_array);
    glGenBuffers(1, &control_vertex_buffer);
    glGenBuffers(1, &control_index_buffer);
    glBindVertexArray(control_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, control_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, control_buffer.vertex_buffer.size() * sizeof(vec3), control_buffer.vertex_buffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, control_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, control_buffer.index_buffer.size() * sizeof(GLuint), control_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // pointer binding
    auto control_vpos_location = glGetAttribLocation(polygon_program, "pos");
    glVertexAttribPointer(static_cast<GLuint>(control_vpos_location), 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(static_cast<GLuint>(control_vpos_location));

    auto control_vertrex_vpos_location = glGetAttribLocation(vertex_program, "pos");
    glVertexAttribPointer(static_cast<GLuint>(control_vertrex_vpos_location), 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(static_cast<GLuint>(control_vertrex_vpos_location));

//    auto vcolor_location = glGetAttribLocation(polygon_program, "color_in");
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
                    surface_resolution.increment();
                    slider_resolution += 1;
                    update_surface_buffer();
                    break;
                case GLFW_KEY_SLASH:
                    surface_resolution.decrement();
                    if (slider_resolution > 1) {
                        slider_resolution -= 1;
                    }
                    update_surface_buffer();
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
            surface_resolution.set(static_cast<uint32_t>(slider_resolution));
            update_surface_buffer();
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
        glMultiDrawElements(
            GL_TRIANGLES,
            surface_buffer.counts.data(),
            GL_UNSIGNED_INT,
            (void**) surface_buffer.indices.data(),
            static_cast<GLsizei>(surface_buffer.counts.size())
        );
    }

    if (control_mode) {
        // Render control polygon
        glDepthMask(GL_FALSE);
        glUseProgram(polygon_program);

        auto vp_location = glGetUniformLocation(polygon_program, "VP");
        auto m_location = glGetUniformLocation(polygon_program, "M");
        auto color_location = glGetUniformLocation(polygon_program, "color_in");
        auto camera_location = glGetUniformLocation(polygon_program, "camera_pos");

        glUniformMatrix4fv(vp_location, 1, GL_FALSE, value_ptr(vp));
        glUniformMatrix4fv(m_location, 1, GL_FALSE, value_ptr(model));
        glUniform3fv(color_location, 1, value_ptr(vec3(1, 0, 0)));
        glUniform3fv(camera_location, 1, value_ptr(camera.get_pos()));

        glBindVertexArray(control_vertex_array);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_LINES, static_cast<GLsizei>(control_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));
        glDepthMask(GL_TRUE);

        // Render vertices
        glUseProgram(vertex_program);

        auto vertex_vp_location = glGetUniformLocation(vertex_program, "VP");
        auto vertex_m_location = glGetUniformLocation(vertex_program, "M");
        auto vertex_color_location = glGetUniformLocation(vertex_program, "color_in");
        auto vertex_camera_location = glGetUniformLocation(vertex_program, "camera_pos");
        auto vertex_camera_up_location = glGetUniformLocation(vertex_program, "camera_up");

        glUniformMatrix4fv(vertex_vp_location, 1, GL_FALSE, value_ptr(vp));
        glUniformMatrix4fv(vertex_m_location, 1, GL_FALSE, value_ptr(model));
        glUniform3fv(vertex_color_location, 1, value_ptr(vec3(0, 0, 1)));
        glUniform3fv(vertex_camera_location, 1, value_ptr(camera.get_pos()));
        glUniform3fv(vertex_camera_up_location, 1, value_ptr(camera.get_up()));

        glBindVertexArray(control_vertex_array);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(control_buffer.vertex_buffer.size()));
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

window::window(window&& window) noexcept : surface_resolution(1) {
    *this = move(window);
}

window& window::operator=(window&& window) noexcept {
    glfw_window = exchange(window.glfw_window, nullptr);
    title = move(window.title);
    width = exchange(window.width, 0);
    height = exchange(window.height, 0);
    polygon_program = exchange(window.polygon_program, 0);
    vertex_program = exchange(window.vertex_program, 0);
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
    surface_resolution = move(window.surface_resolution);
    slider_resolution = exchange(window.slider_resolution, 0);

    return *this;
}

mouse_pos window::get_mouse_pos() const {
    double x_pos;
    double y_pos;
    glfwGetCursorPos(glfw_window, &x_pos, &y_pos);
    return mouse_pos(x_pos, y_pos);
}

void window::load_surface_data_to_gpu() const {
    auto vec_data = surface_buffer.get_combined_vec_data();

    glBindVertexArray(surface_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, surface_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vec_data.size() * sizeof(vec3), vec_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surface_buffer.index_buffer.size() * sizeof(GLuint), surface_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // TODO: perhaps this is not necessary?
    // pointer binding
    auto vpos_location = glGetAttribLocation(phong_program, "pos_in");
    glVertexAttribPointer(static_cast<GLuint>(vpos_location), 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*) 0);
    glEnableVertexAttribArray(static_cast<GLuint>(vpos_location));

    auto vnormal_location = glGetAttribLocation(phong_program, "normal_in");
    glVertexAttribPointer(static_cast<GLuint>(vnormal_location), 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*) (sizeof(vec3)));
    glEnableVertexAttribArray(static_cast<GLuint>(vnormal_location));

//    auto vcolor_location = glGetAttribLocation(polygon_program, "color_in");
//    glVertexAttribPointer(static_cast<GLuint>(vcolor_location), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (sizeof(float) * 3));
//    glEnableVertexAttribArray(static_cast<GLuint>(vcolor_location));
}

void window::update_surface_buffer() {
    // TODO: Get real resolution
    auto grids = tmesh.get_grids(20);
    surface_buffer = get_multi_render_buffer(grids);
    control_buffer = get_buffer(tmesh.mesh);

    load_surface_data_to_gpu();
}

}
