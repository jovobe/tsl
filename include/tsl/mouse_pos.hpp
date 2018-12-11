#ifndef TSL_MOUSE_POS_HPP
#define TSL_MOUSE_POS_HPP

namespace tsl {

struct mouse_pos {
    double x;
    double y;

    mouse_pos(double x, double y) : x(x), y(y) {};
};

}

#endif //TSL_MOUSE_POS_HPP
