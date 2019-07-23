#ifndef TSE_RESOLUTION_HPP
#define TSE_RESOLUTION_HPP

namespace tse {

/**
 * @brief Represents a resolution which can take values between 1 and max value of `T`.
 */
template<typename T>
class resolution {
public:
    explicit resolution(T start) : val(start) {};

    /**
     * @brief Incements the resolution by one.
     */
    void increment();

    /**
     * @brief Decrements the resolution by one. If the resolution is already one, this does nothing.
     */
    void decrement();

    /**
     * @brief Returns the current value of the resolution.
     */
    T get() const;

    /**
     * @brief Sets the current resolution, if the given value is >= 1.
     */
    void set(T val);

    /**
     * @brief Returns a pointer to the resolution value.
     */
    T* data();

private:
    /// Internal value of the resolution.
    T val;
};

}

#include "resolution.tcc"

#endif //TSE_RESOLUTION_HPP
