#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
#include <iterator>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <portable-file-dialogs.h>

#include <fmt/format.h>

#include <tsl/util/println.hpp>
#include <tsl/geometry/tmesh/tmesh.hpp>
#include <tsl/geometry/line.hpp>
#include <tsl/geometry/plane.hpp>
#include <tsl/io/obj.hpp>
#include <tsl/algorithm/generator.hpp>
#include <tsl/evaluation/surface_evaluator.hpp>
#include <tsl/algorithm/reduction.hpp>

#include "tse/rendering/tmesh.hpp"
#include "tse/window.hpp"
#include "tse/application.hpp"
#include "tse/opengl.hpp"

using std::move;
using std::exchange;
using std::string;
using std::vector;
using std::reference_wrapper;
using std::find;
using std::copy_if;
using std::inserter;

using glm::radians;
using glm::fvec3;
using glm::mat4;
using glm::value_ptr;
using glm::perspective;
using glm::lookAt;
using glm::rotate;
using glm::length;

using fmt::format;

using tsl::tmesh_cube;
using tsl::println;
using tsl::panic;
using tsl::line;
using tsl::plane;
using tsl::edge_handle;
using tsl::half_edge_handle;
using tsl::vertex_handle;
using tsl::face_handle;
using tsl::edge_direction;
using tsl::read_obj_into_tmesh;
using tsl::vec4;

namespace tse {

window::window(string&& title, uint32_t width, uint32_t height) :
    title(move(title)),
    width(width),
    height(height),
    wireframe_mode(false),
    control_mode(true),
    surface_mode(true),
    normal_mode(false),
    show_reflection_lines(false),
    move_object(false),
    dialogs(),
    edge_remove_percentage(10.0f),
    surface_resolution(1),
    cube_size(5),
    evaluator(tmesh_cube(static_cast<size_t>(cube_size))),
    camera()
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

    glfw_window = glfw_window_ptr(glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr));
    if (!glfw_window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfw_window.get());
    glfwSetKeyCallback(glfw_window.get(), &application::glfw_key_callback);
    glfwSetFramebufferSizeCallback(glfw_window.get(), &application::glfw_framebuffer_size_callback);
    glfwSetWindowSizeCallback(glfw_window.get(), &application::glfw_window_size_callback);
    glfwSetMouseButtonCallback(glfw_window.get(), &application::glfw_mouse_button_callback);
    glfwSwapInterval(1);

    // GLEW
    if (glewInit() != GLEW_OK) {
        println("ERROR: Failed to init GLEW!");
        exit(EXIT_FAILURE);
    }

    // ImGui
    const char* glsl_version = "#version 330";
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.IniFilename = nullptr;

    ImGui_ImplGlfw_InitForOpenGL(glfw_window.get(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    // get framebuffer size
    glfwGetFramebufferSize(glfw_window.get(), &frame_width, &frame_height);

    // picking
    glGenFramebuffers(1, &picking_frame);
    glBindFramebuffer(GL_FRAMEBUFFER, picking_frame);

    glGenTextures(1, &picking_texture);
    glBindTexture(GL_TEXTURE_2D, picking_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

    glGenRenderbuffers(1, &picking_render);
    glBindRenderbuffer(GL_RENDERBUFFER, picking_render);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, picking_render);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picking_texture, 0);

    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto vertex_shader = create_shader("shader/vertex.glsl", GL_VERTEX_SHADER);
    auto fragment_shader = create_shader("shader/fragment.glsl", GL_FRAGMENT_SHADER);

    auto vertex_picking_shader = create_shader("shader/picking/vertex.glsl", GL_VERTEX_SHADER);
    auto vertex_wogeom_picking_shader = create_shader("shader/picking/vertex_without_geometry.glsl", GL_VERTEX_SHADER);
    auto fragment_picking_shader = create_shader("shader/picking/fragment.glsl", GL_FRAGMENT_SHADER);

    auto edge_geometry_shader = create_shader("shader/polygon/geometry.glsl", GL_GEOMETRY_SHADER);
    auto vertex_geometry_shader = create_shader("shader/vertex/geometry.glsl", GL_GEOMETRY_SHADER);

    auto phong_vertex_shader = create_shader("shader/phong/vertex.glsl", GL_VERTEX_SHADER);
    auto phong_fragment_shader = create_shader("shader/phong/fragment.glsl", GL_FRAGMENT_SHADER);

    auto normal_vertex_shader = create_shader("shader/normal/vertex.glsl", GL_VERTEX_SHADER);
    auto normal_geometry_shader = create_shader("shader/normal/geometry.glsl", GL_GEOMETRY_SHADER);
    auto normal_fragment_shader = create_shader("shader/normal/fragment.glsl", GL_FRAGMENT_SHADER);

    edge_program = create_program({vertex_shader, fragment_shader, edge_geometry_shader});
    vertex_program = create_program({vertex_shader, fragment_shader, vertex_geometry_shader});
    edge_picking_program = create_program({vertex_picking_shader, fragment_picking_shader, edge_geometry_shader});
    vertex_picking_program = create_program({vertex_picking_shader, fragment_picking_shader, vertex_geometry_shader});
    phong_program = create_program({phong_vertex_shader, phong_fragment_shader});
    surface_picking_program = create_program({vertex_wogeom_picking_shader, fragment_picking_shader});
    normal_program = create_program({normal_vertex_shader, normal_geometry_shader, normal_fragment_shader});

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_picking_shader);
    glDeleteShader(fragment_picking_shader);
    glDeleteShader(edge_geometry_shader);
    glDeleteShader(vertex_geometry_shader);
    glDeleteShader(phong_vertex_shader);
    glDeleteShader(phong_fragment_shader);
    glDeleteShader(vertex_wogeom_picking_shader);

    glGenVertexArrays(1, &surface_normal_vertex_array);
    glGenVertexArrays(1, &surface_picking_vertex_array);
    glGenVertexArrays(1, &surface_vertex_array);
    glGenBuffers(1, &surface_vertex_buffer);
    glGenBuffers(1, &surface_index_buffer);
    glGenBuffers(1, &surface_picked_buffer);

    glGenVertexArrays(1, &control_edges_vertex_array);
    glGenVertexArrays(1, &control_picking_edges_vertex_array);
    glGenBuffers(1, &control_edges_vertex_buffer);
    glGenBuffers(1, &control_edges_index_buffer);
    glGenBuffers(1, &edges_picked_buffer);

    glGenVertexArrays(1, &control_vertices_vertex_array);
    glGenVertexArrays(1, &control_picking_vertices_vertex_array);
    glGenBuffers(1, &control_vertices_vertex_buffer);
    glGenBuffers(1, &control_vertices_index_buffer);
    glGenBuffers(1, &vertices_picked_buffer);

    update_buffer();

    glEnable(GL_DEPTH_TEST);
}

void window::glfw_key_callback(int key, int scancode, int action, int mods) {
    switch (action) {
        case GLFW_PRESS:
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(glfw_window.get(), GLFW_TRUE);
                    break;
                case GLFW_KEY_W:
                    if ((mods & GLFW_MOD_CONTROL) != 0) {
                        println("INFO: toggled wireframe mode");
                        wireframe_mode = !wireframe_mode;
                    } else {
                        camera.moving_direction.forward = true;
                    }
                    break;
                case GLFW_KEY_S:
                    if ((mods & GLFW_MOD_CONTROL) != 0) {
                        println("INFO: toggled surface mode");
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
                        println("INFO: toggled control plygon mode");
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
                    update_buffer();
                    break;
                case GLFW_KEY_SLASH:
                    surface_resolution.decrement();
                    update_buffer();
                    break;
                case GLFW_KEY_X:
                    move_direction.x = true;
                    break;
                case GLFW_KEY_Z:
                    move_direction.y = true;
                    break;
                case GLFW_KEY_V:
                    move_direction.z = true;
                    break;
                case GLFW_KEY_LEFT_SHIFT:
                    camera.move_boost = true;
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
                case GLFW_KEY_X:
                    move_direction.x = false;
                    break;
                case GLFW_KEY_Z:
                    move_direction.y = false;
                    break;
                case GLFW_KEY_V:
                    move_direction.z = false;
                    break;
                case GLFW_KEY_LEFT_SHIFT:
                    camera.move_boost = false;
                    break;
                case GLFW_KEY_O:
                    open_file_dialog_and_load_selected_file();
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
    frame_width = width;
    frame_height = height;

    update_texture_sizes();
}

void window::update_texture_sizes() const {
    glBindTexture(GL_TEXTURE_2D, picking_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, frame_width, frame_height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, picking_render);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, frame_width, frame_height);
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
                    glfwSetInputMode(glfw_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    camera.moving_direction.mouse = true;
                    break;
                case GLFW_MOUSE_BUTTON_LEFT: {
                    if (!ImGui::GetIO().WantCaptureMouse) {
                        // if ctrl is pressed, add element to current selections
                        bool single_selection = (mods & GLFW_MOD_CONTROL) == 0;
                        request_pick = request_pick_data(get_mouse_pos(), single_selection);
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        case GLFW_RELEASE:
            switch (button) {
                case GLFW_MOUSE_BUTTON_RIGHT:
                    glfwSetInputMode(glfw_window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    camera.moving_direction.mouse = false;
                    camera.reset_last_move_time();
                    camera.reset_curos_pos();
                    break;
                case GLFW_MOUSE_BUTTON_LEFT: {
                    move_object = false;
                    start_move = nullopt;
                    if (request_remove) {
                        auto elem = *request_remove;
                        if (picked_elements.find(elem) != picked_elements.end()) {
                            picked_elements.erase(elem);
                            update_picked_buffer();
                        }
                        request_remove = nullopt;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void window::render() {
    draw_gui();

    glfwMakeContextCurrent(glfw_window.get());
    camera.handle_moving_direction(get_mouse_pos());

    // projection
    auto projection = perspective(radians(45.0f), static_cast<float>(width) / height, 0.1f, 1000.0f);
    auto view = camera.get_view_matrix();

    auto model = mat4(1.0f);
    auto vp = projection * view;

    // Picking phase
    picking_phase(model, vp);
    handle_object_move(model, vp);

    // Render phase
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (surface_mode) {
        draw_surface(model, vp);
    }
    if (normal_mode) {
        draw_surface_normals(model, vp);
    }
    if (control_mode) {
        draw_control_polygon(model, vp);
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(glfw_window.get());
    glfwSwapBuffers(glfw_window.get());
    glfwPollEvents();
}

void window::picking_phase(const mat4& model, const mat4& vp) {
    if (request_pick) {
        auto data = *request_pick;
        auto id = read_pixel(data.pos);
        if (id != 0) {
            if (data.single_select) {
                picked_elements.clear();
                picked_elements.insert(*picking_map.get_object(id));
            } else {
                auto [it, inserted] = picked_elements.insert(*picking_map.get_object(id));
                // If element already picked, remove selection
                if (!inserted) {
                    request_remove = *it;
                }
            }
            update_picked_buffer();
            move_object = true;
        }

        request_pick = nullopt;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, picking_frame);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (surface_mode) {
        draw_surface_picking(model, vp);
    }
    if (control_mode) {
        draw_control_polygon_picking(model, vp);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, frame_width, frame_height);
}

uint32_t window::read_pixel(const mouse_pos& pos) const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, picking_frame);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    uint32_t data = 0;
    auto x = static_cast<GLint>(pos.x);
    auto y = static_cast<GLint>(height - pos.y);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &data);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    return data;
}

void window::draw_gui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool show_about_popup = false;

    // Menu Bar
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("TSE")) {
        if (ImGui::MenuItem("About TSE")) {
            show_about_popup = true;
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Settings")) {
            dialogs.settings = !dialogs.settings;
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            open_file_dialog_and_load_selected_file();
        }
        if (ImGui::MenuItem("Save As..")) {}
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
        if (ImGui::BeginMenu("Tool Windows")) {
            if (ImGui::MenuItem("Selected Element")) {
                dialogs.selected_elements = !dialogs.selected_elements;
            }
            if (ImGui::MenuItem("Remove Edges")) {
                dialogs.remove_edges = !dialogs.remove_edges;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    if (show_about_popup) {
        ImGui::OpenPopup("abouttse");
    }

    ImGui::SetNextWindowPosCenter();
    if (ImGui::BeginPopup("abouttse")) {

        ImGui::Text("T-Spline Editor");
        ImGui::Text("\n");
        ImGui::Text("Version 1.0.0");
        ImGui::Text("\n");
        ImGui::Text("Developed by: Johan M. von Behren (johan@vonbehren.eu)");
        ImGui::Text("This software is licensed under GNU GPL version 3.");
        ImGui::Text("Source code is available on "); ImGui::SameLine(); ImGui::Selectable("GitHub "); ImGui::SameLine(); ImGui::Text(".");
        ImGui::Text("\n");
        ImGui::Text("Powered by open source software:");
        ImGui::Text("- GLFW");
        ImGui::Text("- GLEW");
        ImGui::Text("- OpenBLAS");
        ImGui::Text("- Google Test");
        ImGui::Text("- GLM");
        ImGui::Text("- Dear ImGui");
        ImGui::Text("- Portable File Dialogs");
        ImGui::Text("- tinyobjloader");

        ImGui::EndPopup();
    }

    if (dialogs.settings) {
        ImGui::SetNextWindowPos(ImVec2(0, 30), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(450, 0), ImVec2(width, height));

        if (ImGui::Begin("Settings", &dialogs.settings)) {
            ImGui::Checkbox("Wireframe mode", &wireframe_mode);
            ImGui::Checkbox("Show control polygon", &control_mode);
            ImGui::Checkbox("Show surface", &surface_mode);
            ImGui::Checkbox("Show surface normals", &normal_mode);
            ImGui::Checkbox("Show reflection lines", &show_reflection_lines);
            ImGui::Checkbox("Prevent broken meshes to be imported", &evaluator.config.panic_at_integrity_violations);

            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
            if (ImGui::InputInt("Resolution", (int*) surface_resolution.data(), 1, 1)) {
                if (surface_resolution.get() < 1) {
                    surface_resolution.set(1);
                }
                update_buffer();
            }

            if (ImGui::InputInt("Cube size", &cube_size, 1, 1)) {
                if (cube_size < 3) {
                    cube_size = 3;
                } else if (cube_size > 20) {
                    cube_size = 20;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Load")) {
                evaluator = surface_evaluator(tmesh_cube(static_cast<size_t>(cube_size)));
                update_buffer();
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
    }

    if (dialogs.remove_edges) {
        ImGui::SetNextWindowPos(ImVec2(0, 30), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(450, 0), ImVec2(width, height));

        if (ImGui::Begin("Remove edges", &dialogs.remove_edges)) {
            if (ImGui::Button("Remove selected edges")) {
                vector<reference_wrapper<const picking_element>> edges_picked;
                copy_if(picked_elements.begin(), picked_elements.end(), back_inserter(edges_picked), [](const picking_element& elem) {
                    return elem.type == object_type::edge;
                });

                bool continue_after_warn = true;
                if (edges_picked.empty()) {
                    pfd::message("Problem", "No edges selected!", pfd::choice::ok, pfd::icon::warning);
                } else {
                    auto button = pfd::message("Warning", format("Are you sure you want to delete: {} edges?", edges_picked.size()), pfd::choice::yes_no, pfd::icon::question).result();
                    continue_after_warn = button == pfd::button::yes;
                }

                if (continue_after_warn) {
                    vector<edge_handle> removed_egdes;
                    for (const auto& edge: edges_picked) {
                        edge_handle handle(edge.get().handle.get_idx());
                        if (evaluator.remove_edge(handle, false)) {
                            removed_egdes.push_back(handle);
                        } else {
                            pfd::message("Problem", format("Edge with id: {} could not be deleted!", handle), pfd::choice::ok, pfd::icon::error);
                        }
                    }

                    if (!removed_egdes.empty()) {

                        // Save picked elements without removed edges
                        set<picking_element> old_picked;
                        copy_if(picked_elements.begin(), picked_elements.end(), inserter(old_picked, old_picked.begin()), [&removed_egdes](const picking_element& elem){
                            if (elem.type == object_type::edge) {
                                edge_handle eh(elem.handle.get_idx());
                                return find(removed_egdes.begin(), removed_egdes.end(), eh) == removed_egdes.end();
                            }
                            return true;
                        });

                        // Update all buffers
                        update_buffer();

                        // Restore the picked elements and update the picked buffer to make them visible
                        picked_elements.insert(old_picked.begin(), old_picked.end());
                        update_picked_buffer();
                    }
                }
            }
            ImGui::Separator();
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::SliderFloat("Percentage of edges to be removed", &edge_remove_percentage, 0.0f, 100.0f, "%.1f %%");
            if (ImGui::Button("Remove % of edges")) {
                bool continue_after_warn = true;
                auto button = pfd::message("Warning", format("Are you sure you want to delete {}% of the edges?", edge_remove_percentage), pfd::choice::yes_no, pfd::icon::question).result();
                continue_after_warn = button == pfd::button::yes;
                if (continue_after_warn) {
                    evaluator.remove_edges(edge_remove_percentage);
                    update_buffer();
                }
            }

            ImGui::End();
        }
    }

    if (dialogs.selected_elements) {
        auto picked_elem_window_width = 200;
        ImGui::SetNextWindowPos(ImVec2(this->width - picked_elem_window_width, 30), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(picked_elem_window_width, 100), ImVec2(width, height));
        const auto& mesh = evaluator.get_tmesh();

        auto draw_edge_information = [&] (const half_edge_handle& eh) {

            if (ImGui::TreeNode((void*)(intptr_t) eh.get_idx(), "half edge: %u", eh.get_idx())) {
                if (ImGui::TreeNode("geometry data")) {
                    ImGui::BulletText("next: %u", mesh.get_next(eh).get_idx());
                    ImGui::BulletText("prev: %u", mesh.get_prev(eh).get_idx());
                    ImGui::BulletText("twin: %u", mesh.get_twin(eh).get_idx());
                    ImGui::BulletText("target: %u", mesh.get_target(eh).get_idx());
                    ImGui::BulletText("face: %u", mesh.get_face_of_half_edge(eh).unwrap().get_idx());
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("t-mesh data")) {
                    // Check for border edge
                    if (mesh.is_border(eh)) {
                        ImGui::BulletText("this half edge lies on the border of the mesh");
                    } else {
                        ImGui::BulletText("points into face corner: %s", *mesh.corner(eh) ? "true" : "false");
                        ImGui::BulletText("knot interval: %.2f", *mesh.get_knot_interval(eh));
                        const auto& coords = evaluator.get_coord_map()[eh];
                        ImGui::BulletText("local coords (uv) of vertex for current half edge: (%.0f, %.0f)", coords.x, coords.y);
                        ImGui::BulletText("direction (dir) of vertex for current half edge: %u", evaluator.get_dir_map()[eh]);
                        auto& trans = evaluator.get_edge_trans_map()[eh];
                        ImGui::BulletText("transition: scale: %.2f, rotate: %u, translate: (%.2f, %.2f)", trans.f, trans.r, trans.t.x, trans.t.y);
                    }

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
        };

        if (ImGui::Begin("Selected elements", &dialogs.selected_elements)) {
            ImGui::Text("Count: %lu", picked_elements.size());
            ImGui::Separator();
            for (const auto& elem: picked_elements) {

                switch (elem.type) {
                    case object_type::vertex: {
                        vertex_handle vh(elem.handle.get_idx());
                        if (ImGui::TreeNode((void*)(intptr_t) vh.get_idx(),"Vertex (id: %u)", vh.get_idx())) {
                            ImGui::BulletText("extended valence: %lu", mesh.get_extended_valence(vh));
                            auto pos = mesh.get_vertex_position(vh);
                            ImGui::BulletText("pos: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
                            auto out = mesh.get_out(vh);
                            if (out) {
                                ImGui::BulletText("out: %u", mesh.get_out(vh).unwrap().get_idx());
                            } else {
                                ImGui::BulletText("out: none");
                            }
                            ImGui::Separator();

                            if (ImGui::TreeNode("ingoing half edges (cw order)")) {
                                for (const auto& eh: mesh.get_half_edges_of_vertex(vh, edge_direction::ingoing)) {
                                    draw_edge_information(eh);
                                }
                                ImGui::TreePop();
                            }
                            ImGui::TreePop();
                        }

                        break;
                    }
                    case object_type::edge: {
                        edge_handle eh(elem.handle.get_idx());
                        if (ImGui::TreeNode((void*)(intptr_t) eh.get_idx(), "Edge (id: %u)", eh.get_idx())) {
                            ImGui::Separator();

                            if (ImGui::TreeNode("consisting of half edges")) {
                                for (const auto& heh: mesh.get_half_edges_of_edge(eh)) {
                                    draw_edge_information(heh);
                                }
                                ImGui::TreePop();
                            }
                            ImGui::TreePop();
                        }

                        break;
                    }
                    case object_type::face: {
                        face_handle fh(elem.handle.get_idx());
                        if (ImGui::TreeNode((void*)(intptr_t) fh.get_idx(), "Face (id: %u)", elem.handle.get_idx())) {
                            auto max_local_coords = evaluator.get_max_coords(fh);
                            ImGui::BulletText("local coordinates: (%.2f, %.2f)", max_local_coords.x, max_local_coords.y);
                            ImGui::BulletText("edge: %u", mesh.get_edge(fh).get_idx());

                            ImGui::Separator();

                            const auto& support = evaluator.get_support_map()[fh];
                            if (ImGui::TreeNode((void*) nullptr, "supporting basis functions: (%lu)", support.size())) {
                                for (const auto& [vh, index, trans]: support) {
                                    if (ImGui::TreeNode((void*)(intptr_t) vh.get_idx(), "vertex: %u", vh.get_idx())) {
                                        auto [uv, vv] = evaluator.get_knot_vectors()[vh][index];
                                        if (ImGui::TreeNode("knot vector u")) {
                                            for (const auto& u: uv) {
                                                ImGui::BulletText("%.2f", u);
                                            }
                                            ImGui::TreePop();
                                        }
                                        if (ImGui::TreeNode("knot vector v")) {
                                            for (const auto& v: vv) {
                                                ImGui::BulletText("%.2f", v);
                                            }
                                            ImGui::TreePop();
                                        }
                                        ImGui::TreePop();
                                    }
                                }
                                ImGui::TreePop();
                            }
                            ImGui::TreePop();
                        }

                        break;
                    }
                    default:
                        panic("unknown object type picked!");
                }
            }

            if (!picked_elements.empty()) {
                if (ImGui::Button("Clear (deselect all)")) {
                    picked_elements.clear();
                    update_picked_buffer();
                }
            }

            ImGui::End();
        }
    }

    ImGui::Render();
}

void window::draw_control_polygon_picking(const mat4& model, const mat4& vp) const {
    // Render control polygon
    glUseProgram(edge_picking_program);

    auto vp_location = glGetUniformLocation(edge_picking_program, "VP");
    auto m_location = glGetUniformLocation(edge_picking_program, "M");
    auto camera_location = glGetUniformLocation(edge_picking_program, "camera_pos");

    glUniformMatrix4fv(vp_location, 1, GL_FALSE, value_ptr(vp));
    glUniformMatrix4fv(m_location, 1, GL_FALSE, value_ptr(model));
    glUniform3fv(camera_location, 1, value_ptr(camera.get_pos()));

    glBindVertexArray(control_picking_edges_vertex_array);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_LINES, static_cast<GLsizei>(control_edges_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));

    // Render vertices
    glDepthFunc(GL_ALWAYS);
    glUseProgram(vertex_picking_program);

    auto vertex_vp_location = glGetUniformLocation(vertex_picking_program, "VP");
    auto vertex_m_location = glGetUniformLocation(vertex_picking_program, "M");
    auto vertex_camera_location = glGetUniformLocation(vertex_picking_program, "camera_pos");
    auto vertex_camera_up_location = glGetUniformLocation(vertex_picking_program, "camera_up");

    glUniformMatrix4fv(vertex_vp_location, 1, GL_FALSE, value_ptr(vp));
    glUniformMatrix4fv(vertex_m_location, 1, GL_FALSE, value_ptr(model));
    glUniform3fv(vertex_camera_location, 1, value_ptr(camera.get_pos()));
    glUniform3fv(vertex_camera_up_location, 1, value_ptr(camera.get_up()));

    glBindVertexArray(control_picking_vertices_vertex_array);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_POINTS, static_cast<GLsizei>(control_vertices_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));
    glDepthFunc(GL_LESS);
}

void window::draw_control_polygon(const mat4& model, const mat4& vp) const {
    // Render control polygon
    glDepthMask(GL_FALSE);
    glUseProgram(edge_program);

    auto vp_location = glGetUniformLocation(edge_program, "VP");
    auto m_location = glGetUniformLocation(edge_program, "M");
    auto color_location = glGetUniformLocation(edge_program, "color_in");
    auto camera_location = glGetUniformLocation(edge_program, "camera_pos");

    glUniformMatrix4fv(vp_location, 1, GL_FALSE, value_ptr(vp));
    glUniformMatrix4fv(m_location, 1, GL_FALSE, value_ptr(model));
    glUniform3fv(color_location, 1, value_ptr(fvec3(1, 0, 0)));
    glUniform3fv(camera_location, 1, value_ptr(camera.get_pos()));

    glBindVertexArray(control_edges_vertex_array);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_LINES, static_cast<GLsizei>(control_edges_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));
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
    glUniform3fv(vertex_color_location, 1, value_ptr(fvec3(0, 0, 1)));
    glUniform3fv(vertex_camera_location, 1, value_ptr(camera.get_pos()));
    glUniform3fv(vertex_camera_up_location, 1, value_ptr(camera.get_up()));

    glBindVertexArray(control_vertices_vertex_array);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_POINTS, static_cast<GLsizei>(control_vertices_buffer.index_buffer.size()), GL_UNSIGNED_INT, (void*) (sizeof(float) * 0));
}

void window::draw_surface(const mat4& model, const mat4& vp) const {
    glUseProgram(phong_program);

    auto vp_location_phong = glGetUniformLocation(phong_program, "VP");
    auto m_location_phong = glGetUniformLocation(phong_program, "M");
    auto color_location_phong = glGetUniformLocation(phong_program, "color_in");
    auto camera_location = glGetUniformLocation(phong_program, "camera_pos");
    auto reflection_lines_location = glGetUniformLocation(phong_program, "show_reflection_lines");

    glUniformMatrix4fv(vp_location_phong, 1, GL_FALSE, value_ptr(vp));
    glUniformMatrix4fv(m_location_phong, 1, GL_FALSE, value_ptr(model));
    glUniform3fv(color_location_phong, 1, value_ptr(fvec3(0, 1, 0)));
    glUniform3fv(camera_location, 1, value_ptr(camera.get_pos()));
    glUniform1ui(reflection_lines_location, show_reflection_lines);

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

void window::draw_surface_normals(const mat4& model, const mat4& vp) const {
    glUseProgram(normal_program);

    auto vp_location = glGetUniformLocation(normal_program, "VP");
    auto m_location = glGetUniformLocation(normal_program, "M");
    auto color_location = glGetUniformLocation(normal_program, "color_in");
    auto camera_location = glGetUniformLocation(normal_program, "camera_pos");

    glUniformMatrix4fv(vp_location, 1, GL_FALSE, value_ptr(vp));
    glUniformMatrix4fv(m_location, 1, GL_FALSE, value_ptr(model));
    glUniform3fv(color_location, 1, value_ptr(fvec3(0, 0, 1)));
    glUniform3fv(camera_location, 1, value_ptr(camera.get_pos()));

    glBindVertexArray(surface_normal_vertex_array);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glMultiDrawElements(
        GL_POINTS,
        surface_buffer.counts.data(),
        GL_UNSIGNED_INT,
        (void**) surface_buffer.indices.data(),
        static_cast<GLsizei>(surface_buffer.counts.size())
    );
}

void window::draw_surface_picking(const mat4& model, const mat4& vp) const {
    glUseProgram(surface_picking_program);

    auto vp_location_phong = glGetUniformLocation(phong_program, "VP");
    auto m_location_phong = glGetUniformLocation(phong_program, "M");

    glUniformMatrix4fv(vp_location_phong, 1, GL_FALSE, value_ptr(vp));
    glUniformMatrix4fv(m_location_phong, 1, GL_FALSE, value_ptr(model));

    glBindVertexArray(surface_picking_vertex_array);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glMultiDrawElements(
        GL_TRIANGLES,
        surface_buffer.counts.data(),
        GL_UNSIGNED_INT,
        (void**) surface_buffer.indices.data(),
        static_cast<GLsizei>(surface_buffer.counts.size())
    );
}

window::~window() {
    // if this window was moved, we don't have to destruct it
    if (glfw_window) {
        glDeleteVertexArrays(1, &surface_vertex_array);
        glDeleteBuffers(1, &surface_vertex_buffer);
        glDeleteBuffers(1, &surface_index_buffer);

        glDeleteVertexArrays(1, &control_edges_vertex_array);
        glDeleteVertexArrays(1, &control_vertices_vertex_array);
        glDeleteVertexArrays(1, &control_picking_edges_vertex_array);
        glDeleteVertexArrays(1, &control_picking_vertices_vertex_array);
        glDeleteVertexArrays(1, &surface_picking_vertex_array);
        glDeleteVertexArrays(1, &surface_normal_vertex_array);

        glDeleteBuffers(1, &control_edges_vertex_buffer);
        glDeleteBuffers(1, &control_edges_index_buffer);
        glDeleteBuffers(1, &control_vertices_vertex_buffer);
        glDeleteBuffers(1, &control_vertices_index_buffer);
        glDeleteBuffers(1, &surface_picked_buffer);
        glDeleteBuffers(1, &edges_picked_buffer);
        glDeleteBuffers(1, &vertices_picked_buffer);

        glDeleteFramebuffers(1, &picking_frame);
        glDeleteRenderbuffers(1, &picking_render);
        glDeleteTextures(1, &picking_texture);

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

bool window::should_close() const {
    return (glfwWindowShouldClose(glfw_window.get()) != 0);
}

mouse_pos window::get_mouse_pos() const {
    double x_pos;
    double y_pos;
    glfwGetCursorPos(glfw_window.get(), &x_pos, &y_pos);
    return mouse_pos(x_pos, y_pos);
}

void window::update_surface_buffer() {
    tmesh_faces = evaluator.eval_per_face(surface_resolution.get());
    surface_buffer = get_multi_render_buffer(tmesh_faces, picking_map);

    auto vec_data = surface_buffer.get_combined_vec_data();

    glBindVertexArray(surface_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, surface_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vec_data.size() * sizeof(vertex_element), vec_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surface_buffer.index_buffer.size() * sizeof(GLuint), surface_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // pointer binding
    auto vpos_location = static_cast<GLuint>(glGetAttribLocation(phong_program, "pos_in"));
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, pos));
    glEnableVertexAttribArray(vpos_location);

    auto vnormal_location = static_cast<GLuint>(glGetAttribLocation(phong_program, "normal_in"));
    glVertexAttribPointer(vnormal_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, normal));
    glEnableVertexAttribArray(vnormal_location);

    glBindVertexArray(surface_picking_vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface_index_buffer);

    auto surface_picking_vpos_location = static_cast<GLuint>(glGetAttribLocation(surface_picking_program, "pos"));
    glVertexAttribPointer(surface_picking_vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, pos));
    glEnableVertexAttribArray(surface_picking_vpos_location);

    auto surface_picking_vpicking_id_location = static_cast<GLuint>(glGetAttribLocation(surface_picking_program, "picking_id_in"));
    glVertexAttribIPointer(surface_picking_vpicking_id_location, 1, GL_UNSIGNED_INT, sizeof(vertex_element), (void*) offsetof(vertex_element, picking_index));
    glEnableVertexAttribArray(surface_picking_vpicking_id_location);

    glBindVertexArray(surface_normal_vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface_index_buffer);

    auto normal_vpos_location = static_cast<GLuint>(glGetAttribLocation(normal_program, "pos_in"));
    glVertexAttribPointer(normal_vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, pos));
    glEnableVertexAttribArray(normal_vpos_location);

    auto normal_vnormal_location = static_cast<GLuint>(glGetAttribLocation(normal_program, "normal_in"));
    glVertexAttribPointer(normal_vnormal_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, normal));
    glEnableVertexAttribArray(normal_vnormal_location);
}

void window::update_control_buffer() {
    control_edges_buffer = get_edges_buffer(evaluator.get_tmesh(), picking_map);
    control_vertices_buffer = get_vertices_buffer(evaluator.get_tmesh(), picking_map);

    // control edges polygon
    glBindVertexArray(control_edges_vertex_array);

    auto combined_control_edges_data = control_edges_buffer.get_combined_vec_data();
    glBindBuffer(GL_ARRAY_BUFFER, control_edges_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, combined_control_edges_data.size() * sizeof(vertex_element), combined_control_edges_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, control_edges_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, control_edges_buffer.index_buffer.size() * sizeof(GLuint), control_edges_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // pointer binding
    auto control_vpos_location = static_cast<GLuint>(glGetAttribLocation(edge_program, "pos"));
    glVertexAttribPointer(control_vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, pos));
    glEnableVertexAttribArray(control_vpos_location);

    glBindVertexArray(control_picking_edges_vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, control_edges_index_buffer);

    auto control_picking_vpos_location = static_cast<GLuint>(glGetAttribLocation(edge_picking_program, "pos"));
    glVertexAttribPointer(control_picking_vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, pos));
    glEnableVertexAttribArray(control_picking_vpos_location);

    auto control_picking_vpicking_id_location = static_cast<GLuint>(glGetAttribLocation(edge_picking_program, "picking_id_in"));
    glVertexAttribIPointer(control_picking_vpicking_id_location, 1, GL_UNSIGNED_INT, sizeof(vertex_element), (void*) offsetof(vertex_element, picking_index));
    glEnableVertexAttribArray(control_picking_vpicking_id_location);

    // control vertices polygon
    glBindVertexArray(control_vertices_vertex_array);

    auto combined_control_vertices_data = control_vertices_buffer.get_combined_vec_data();
    glBindBuffer(GL_ARRAY_BUFFER, control_vertices_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, combined_control_vertices_data.size() * sizeof(vertex_element), combined_control_vertices_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, control_vertices_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, control_vertices_buffer.index_buffer.size() * sizeof(GLuint), control_vertices_buffer.index_buffer.data(), GL_STATIC_DRAW);

    // pointer binding
    auto control_vertrex_vpos_location = static_cast<GLuint>(glGetAttribLocation(vertex_program, "pos"));
    glVertexAttribPointer(control_vertrex_vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, pos));
    glEnableVertexAttribArray(control_vertrex_vpos_location);

    glBindVertexArray(control_picking_vertices_vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, control_vertices_index_buffer);

    auto control_vertrex_picking_vpos_location = static_cast<GLuint>(glGetAttribLocation(vertex_picking_program, "pos"));
    glVertexAttribPointer(control_vertrex_picking_vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_element), (void*) offsetof(vertex_element, pos));
    glEnableVertexAttribArray(control_vertrex_picking_vpos_location);

    auto control_vertrex_picking_vpicking_id_location = static_cast<GLuint>(glGetAttribLocation(vertex_picking_program, "picking_id_in"));
    glVertexAttribIPointer(control_vertrex_picking_vpicking_id_location, 1, GL_UNSIGNED_INT, sizeof(vertex_element), (void*) offsetof(vertex_element, picking_index));
    glEnableVertexAttribArray(control_vertrex_picking_vpicking_id_location);
}

void window::update_picked_buffer()
{
    // Edges
    auto picked_edges = get_picked_edges_buffer(evaluator.get_tmesh(), picked_elements);

    glBindVertexArray(control_edges_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, edges_picked_buffer);
    glBufferData(GL_ARRAY_BUFFER, picked_edges.size() * sizeof(uint8_t), picked_edges.data(), GL_STATIC_DRAW);

    auto picked_edges_location = static_cast<GLuint>(glGetAttribLocation(edge_program, "picked_in"));
    glVertexAttribIPointer(picked_edges_location, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*) nullptr);
    glEnableVertexAttribArray(picked_edges_location);

    // Vertices
    auto picked_vertices = get_picked_vertices_buffer(evaluator.get_tmesh(), picked_elements);

    glBindVertexArray(control_vertices_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_picked_buffer);
    glBufferData(GL_ARRAY_BUFFER, picked_vertices.size() * sizeof(uint8_t), picked_vertices.data(), GL_STATIC_DRAW);

    auto picked_vertices_location = static_cast<GLuint>(glGetAttribLocation(vertex_program, "picked_in"));
    glVertexAttribIPointer(picked_vertices_location, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*) nullptr);
    glEnableVertexAttribArray(picked_vertices_location);

    // Faces
    auto picked_faces = get_picked_faces_buffer(tmesh_faces, picked_elements);

    glBindVertexArray(surface_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, surface_picked_buffer);
    glBufferData(GL_ARRAY_BUFFER, picked_faces.size() * sizeof(uint8_t), picked_faces.data(), GL_STATIC_DRAW);

    auto picked_faces_location = static_cast<GLuint>(glGetAttribLocation(phong_program, "picked_in"));
    glVertexAttribIPointer(picked_faces_location, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*) nullptr);
    glEnableVertexAttribArray(picked_faces_location);
}

void window::update_buffer()
{
    // This needs to be cleared because the picking map will be cleared
    // otherwise the old indices stored here are invalid
    picked_elements.clear();

    picking_map.clear();
    update_surface_buffer();
    update_control_buffer();
    update_picked_buffer();
}

void window::handle_object_move(const mat4& model, const mat4& vp) {
    if (picked_elements.empty() || !move_object) {
        return;
    }

    // Collect all vertex handles which should be moved
    set<vertex_handle> vertices_to_move;
    for (const auto& elem: picked_elements) {
        switch (elem.type) {
            case object_type::vertex: {
                vertices_to_move.emplace(elem.handle.get_idx());
                break;
            }
            case object_type::edge: {
                edge_handle eh(elem.handle.get_idx());
                evaluator.get_tmesh().get_vertices_of_edge(eh, vertices_to_move);
                break;
            }
            case object_type::face: {
                face_handle fh(elem.handle.get_idx());
                evaluator.get_tmesh().get_vertices_of_face(fh, vertices_to_move);
                break;
            }
            default:
                break;
        }
    }

    // Calc the support vector of the move plane
    vector<reference_wrapper<vec3>> positions;
    positions.reserve(vertices_to_move.size());
    vec3 center(0, 0, 0);
    for (const auto& vh: vertices_to_move) {
        auto& pos = evaluator.get_vertex_pos(vh);
        positions.emplace_back(pos);
        center += pos;
    }
    center /= positions.size();

    // Create plane and ray
    line ray(camera.get_pos(), get_ray(get_mouse_pos(), vp));
    plane move_plane(center, camera.get_direction());

    // Cast ray and move all points by the offset between the last intersection and the current
    auto intersection = *(ray.intersect(move_plane));
    if (!start_move) {
        start_move = intersection;
    } else {
        auto offset = *start_move - intersection;

        // Check for axis aligned movement
        if (move_direction.x) {
            offset *= vec3(1, 0, 0);
        } else if (move_direction.y) {
            offset *= vec3(0, 1, 0);
        } else if (move_direction.z) {
            offset *= vec3(0, 0, 1);
        }

        // If we actually moved, delete the request to deselect the clicked element
        if (length(offset) > 0) {
            request_remove = nullopt;
        }

        // Actually move points
        for (const auto& pos: positions) {
            pos.get() -= offset;
        }
        start_move = intersection;
    }

    // Save picked elements
    set<picking_element> old_picked(picked_elements);

    // Update all buffers
    update_buffer();

    // Restore the picked elements and update the picked buffer to make them visible
    picked_elements.insert(old_picked.begin(), old_picked.end());
    update_picked_buffer();
}

vec3 window::get_ray(const mouse_pos& mouse_pos, const mat4& vp) const {
    float mouse_x = mouse_pos.x / (width  * 0.5f) - 1.0f;
    float mouse_y = mouse_pos.y / (height * 0.5f) - 1.0f;

    mat4 inv_mvp = inverse(vp);
    auto screen_pos = vec4(mouse_x, -mouse_y, 1.0f, 1.0f);
    auto world_pos = inv_mvp * screen_pos;

    return normalize(vec3(world_pos));
}

void window::open_file_dialog_and_load_selected_file() {
    auto files = pfd::open_file("Select a file").result();
    if (!files.empty()) {
        try {
            evaluator = surface_evaluator(read_obj_into_tmesh(files[0]));
            update_buffer();
        } catch(const exception& e) {
            pfd::message("Problem", format("An error occurred while loading: {}\n{}", files[0], e.what()), pfd::choice::ok, pfd::icon::error);
        }
    }
}

}
