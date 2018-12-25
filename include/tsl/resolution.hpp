#ifndef TSL_RESOLUTION_HPP
#define TSL_RESOLUTION_HPP

namespace tsl {

template<typename T>
class resolution {
public:
    resolution() : data(1) {};
    void increment();
    void decrement();
    T get();

private:
    T data;
};

}

#include <tsl/resolution.tcc>

#endif //TSL_RESOLUTION_HPP
