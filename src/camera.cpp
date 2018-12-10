#include <tsl/camera.hpp>
#include <tsl/application.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using glm::lookAt;
using glm::radians;
using glm::normalize;
using glm::pi;
using glm::cross;
using glm::rotateX;
using glm::rotateY;
using glm::dot;
using glm::abs;

using std::cout;
using std::endl;
using std::nullopt;

namespace tsl {

const vec3 camera::INITIAL_POS(0, 0, 3);
const vec3 camera::INITIAL_DIRECTION(0, 0, -1);
const vec3 camera::INITIAL_UP(0, 1, 0);

mat4 camera::get_view_matrix() const {
    return lookAt(pos, pos + direction, up);
}

void camera::cursor_pos_changed(double x_pos, double y_pos) {
    auto time = application::get_instance().get_time();

    // if mouse pos is not initialised, dont do anything except initialising it
    if (!last_cursor_pos) {
        last_cursor_pos = mouse_pos(x_pos, y_pos);
        last_mouse_time = time;
        return;
    }

    // calc mouse movement
    double time_delta = time - last_mouse_time;
    double x_pos_delta = (*last_cursor_pos).x - x_pos;
    double y_pos_delta = (*last_cursor_pos).y - y_pos;
    auto x_offset = x_pos_delta * MOUSE_SENSITIVITY * time_delta;
    auto y_offset = y_pos_delta * MOUSE_SENSITIVITY * time_delta;
    last_mouse_time = time;
    (*last_cursor_pos).x = x_pos;
    (*last_cursor_pos).y = y_pos;

    // TODO: camera movement inverts, when crossing x axis

    // constraint view to not turn upside down
    auto new_direction = rotateY(rotateX(direction, static_cast<float>(y_offset)), static_cast<float>(x_offset));
    if (abs(dot(new_direction, up)) < 0.9) {
        direction = new_direction;
    }
}

void camera::reset_curos_pos() {
    last_cursor_pos = nullopt;
}

void camera::handle_moving_direction() {
    auto time = application::get_instance().get_time();
    if (!last_move_time) {
        last_move_time = time;
        return;
    }

    auto time_delta = static_cast<float>(time - (*last_move_time));
    last_move_time = time;

    vec3 move(0, 0, 0);
    if (moving_direction.forward) {
        move += direction * time_delta * MOVE_SPEED;
    }
    if (moving_direction.backwards) {
        move -= direction * time_delta * MOVE_SPEED;
    }
    if (moving_direction.left) {
        auto left = cross(up, direction);
        move += left * time_delta * MOVE_SPEED;
    }
    if (moving_direction.right) {
        auto right = cross(direction, up);
        move += right * time_delta * MOVE_SPEED;
    }
    if (moving_direction.up) {
        move += up * time_delta * MOVE_SPEED;
    }
    if (moving_direction.down) {
        move -= up * time_delta * MOVE_SPEED;
    }
    pos += move;
}

void camera::reset_last_move_time() {
    last_move_time = nullopt;
}

void camera::reset_position() {
    pos = INITIAL_POS;
    direction = INITIAL_DIRECTION;
    up = INITIAL_UP;
}

}
