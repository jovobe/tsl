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
using glm::rotate;
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

void camera::reset_curos_pos() {
    last_cursor_pos = nullopt;
}

void camera::handle_moving_direction(const mouse_pos& mouse_pos) {
    auto time = application::get_instance().get_time();
    if (!last_move_time) {
        last_move_time = time;
        return;
    }

    auto time_delta = static_cast<float>(time - (*last_move_time));
    auto left = cross(up, direction);
    auto right = cross(direction, up);
    last_move_time = time;

    // handle movement
    vec3 move(0, 0, 0);
    if (moving_direction.forward) {
        move += direction * time_delta * MOVE_SPEED;
    }
    if (moving_direction.backwards) {
        move -= direction * time_delta * MOVE_SPEED;
    }
    if (moving_direction.left) {
        move += left * time_delta * MOVE_SPEED;
    }
    if (moving_direction.right) {
        move += right * time_delta * MOVE_SPEED;
    }
    if (moving_direction.up) {
        move += up * time_delta * MOVE_SPEED;
    }
    if (moving_direction.down) {
        move -= up * time_delta * MOVE_SPEED;
    }
    pos += move;

    // handle orientation
    if (moving_direction.mouse) {
        // if mouse pos is not initialised, dont do anything except initialising it
        if (!last_cursor_pos) {
            last_cursor_pos = mouse_pos;
            return;
        }

        // calc mouse movement
        double x_pos_delta = (*last_cursor_pos).x - mouse_pos.x;
        double y_pos_delta = (*last_cursor_pos).y - mouse_pos.y;
        auto x_offset = x_pos_delta * MOUSE_SENSITIVITY * time_delta;
        auto y_offset = y_pos_delta * MOUSE_SENSITIVITY * time_delta;
        last_cursor_pos = mouse_pos;

        // constraint view to not turn upside down
        auto new_direction = rotate(
            rotate(direction, static_cast<float>(y_offset), right),
            static_cast<float>(x_offset),
            up
        );
        if (abs(dot(new_direction, up)) < 0.9) {
            direction = new_direction;
        }
    }
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
