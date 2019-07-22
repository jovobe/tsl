#ifndef TSL_CAMERA_HPP
#define TSL_CAMERA_HPP

#include <optional>

#include <glm/glm.hpp>

#include "tsl/mouse_pos.hpp"

using std::optional;

using glm::fvec3;
using glm::mat4;

namespace tsl {

/**
 * @brief Represents the state of movement commands which are currently send to the application.
 */
struct move_direction {
    bool forward;
    bool backwards;
    bool left;
    bool right;
    bool up;
    bool down;
    bool mouse;

    move_direction():
        forward(false),
        backwards(false),
        left(false),
        right(false),
        up(false),
        down(false),
        mouse(false)
    {};
};

/**
 * @brief Represents the camera in a scene.
 */
class camera {
public:
    /// The directions in which the camera is currently moving.
    move_direction moving_direction;

    /// True, if the camera should move fast.
    bool move_boost;

    camera() :
        moving_direction(),
        move_boost(false),
        pos(INITIAL_POS),
        direction(INITIAL_DIRECTION),
        up(INITIAL_UP),
        last_cursor_pos(),
        last_move_time()
    {};

    /**
     * @brief Returns the view matrix for the current camera position.
     */
    mat4 get_view_matrix() const;

    /**
     * @brief Adjusts the cameras moving direction according to the given mouse input.
     */
    void handle_moving_direction(const mouse_pos& mouse_pos);

    /**
     * @brief Resets the cursor position to none.
     */
    void reset_curos_pos();

    /**
     * @brief Resets the last move time to none.
     */
    void reset_last_move_time();

    /**
     * @brief Resets the 3d position of the camera to `INITIAL_POS`.
     */
    void reset_position();

    /**
     * @brief Returns the 3d position of the camera in the scene.
     */
    fvec3 get_pos() const;

    /**
     * @brief Returns the up vector of the camera.
     */
    fvec3 get_up() const;

    /**
     * @brief Returns the direction vector of the camera.
     */
    fvec3 get_direction() const;

private:
    /// Current camera position.
    fvec3 pos;
    /// Current camera view direction.
    fvec3 direction;
    /// Current cameras up vector.
    fvec3 up;

    /// Last cursor position. If this is none, the user hast not clicked.
    optional<mouse_pos> last_cursor_pos;
    /// Last time the camera was moved. If this is none, the user has not moved the camera.
    optional<double> last_move_time;

    /// Sensitivity for camera view direction change.
    inline static const float MOUSE_SENSITIVITY = 0.1;
    /// Speed for camera movement.
    inline static const float MOVE_SPEED = 3;

    /// Initial camera position in the scene.
    inline static const fvec3 INITIAL_POS = fvec3(0, 0, 3);
    /// Initial camera view direction.
    inline static const fvec3 INITIAL_DIRECTION = fvec3(0, 0, -1);
    /// Initial up vector.
    inline static const fvec3 INITIAL_UP = fvec3(0, 1, 0);
};

}

#endif //TSL_CAMERA_HPP
