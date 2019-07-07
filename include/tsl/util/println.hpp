#ifndef TSL_PRINTLN_HPP
#define TSL_PRINTLN_HPP

#include <iostream>

#include <fmt/core.h>

using std::endl;

using fmt::print;

namespace tsl
{

/**
 * @brief Prints the given string formated like `fmt::print` but adds a "\n" after it.
 */
template <typename S, typename... Args>
inline void println(const S& format_str, const Args& ... args) {
    print(format_str, args...);
    print("\n");
}

}

#endif //TSL_PRINTLN_HPP
