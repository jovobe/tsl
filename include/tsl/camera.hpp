#ifndef TSL_CAMERA_HPP
#define TSL_CAMERA_HPP

#include <glm/glm.hpp>
#include <optional>

using glm::vec3;
using glm::mat4;
using std::optional;

namespace tsl {

struct mouse_pos {
    double x;
    double y;

    mouse_pos(double x, double y) : x(x), y(y) {};
};

class camera {
public:
    camera() :
        pos(0, 0, 3),
        direction(0, 0, -1),
        up(0, 1, 0),
        last_cursor_pos(),
        last_mouse_time(0),
        last_move_time(0)
    {};

    mat4 get_view_matrix() const;
    void move_forward();
    void move_backwards();
    void move_left();
    void move_right();
    void cursor_pos_changed(double x_pos, double y_pos);
    void reset_curos_pos();
    void reset_move_time();

private:
    vec3 pos;
    vec3 direction;
    vec3 up;

    optional<mouse_pos> last_cursor_pos;
    double last_mouse_time;
    double last_move_time;

    const float MOUSE_SENSITIVITY = 0.1;
    const float MOVE_SPEED = 3;
};

}

#endif //TSL_CAMERA_HPP
