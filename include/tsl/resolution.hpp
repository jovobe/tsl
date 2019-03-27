#ifndef TSL_RESOLUTION_HPP
#define TSL_RESOLUTION_HPP

namespace tsl {

template<typename T>
class resolution {
public:
    explicit resolution(T start) : val(start) {};
    void increment();
    void decrement();
    T get() const;
    void set(T val);
    T* data();

private:
    T val;
};

}

#include <tsl/resolution.tcc>

#endif //TSL_RESOLUTION_HPP
