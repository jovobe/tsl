#ifndef TSE_MOUSE_POS_HPP
#define TSE_MOUSE_POS_HPP

namespace tse {

/**
 * @brief Represents the position of the mouse cursor.
 */
struct mouse_pos {
    double x;
    double y;

    mouse_pos(double x, double y) : x(x), y(y) {};
};

}

#endif //TSE_MOUSE_POS_HPP
