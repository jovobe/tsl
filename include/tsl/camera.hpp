#ifndef TSL_CAMERA_HPP
#define TSL_CAMERA_HPP

#include <tsl/mouse_pos.hpp>

#include <glm/glm.hpp>

#include <optional>

using glm::fvec3;
using glm::mat4;
using std::optional;

namespace tsl {

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

class camera {
public:
    move_direction moving_direction;

    camera() :
        pos(INITIAL_POS),
        direction(INITIAL_DIRECTION),
        up(INITIAL_UP),
        last_cursor_pos(),
        last_mouse_time(0),
        last_move_time(),
        moving_direction()
    {};

    mat4 get_view_matrix() const;
    void handle_moving_direction(const mouse_pos& mouse_pos);
    void reset_curos_pos();
    void reset_last_move_time();
    void reset_position();
    fvec3 get_pos() const;
    fvec3 get_up() const;
    fvec3 get_direction() const;

private:
    fvec3 pos;
    fvec3 direction;
    fvec3 up;

    optional<mouse_pos> last_cursor_pos;
    double last_mouse_time;
    optional<double> last_move_time;

    inline static const float MOUSE_SENSITIVITY = 0.1;
    inline static const float MOVE_SPEED = 3;

    inline static const fvec3 INITIAL_POS = fvec3(0, 0, 3);
    inline static const fvec3 INITIAL_DIRECTION = fvec3(0, 0, -1);
    inline static const fvec3 INITIAL_UP = fvec3(0, 1, 0);
};

}

#endif //TSL_CAMERA_HPP
