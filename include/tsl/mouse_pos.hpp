#ifndef TSL_MOUSE_POS_HPP
#define TSL_MOUSE_POS_HPP

namespace tsl {

/**
 * @brief Represents the position of the mouse cursor.
 */
struct mouse_pos {
    double x;
    double y;

    mouse_pos(double x, double y) : x(x), y(y) {};
};

}

#endif //TSL_MOUSE_POS_HPP
