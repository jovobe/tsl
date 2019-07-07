#ifndef TSL_PANIC_HPP
#define TSL_PANIC_HPP

#include <utility>
#include <exception>
#include <string>
#include <optional>

#include <fmt/format.h>

using std::move;
using std::exception;
using std::string;
using std::optional;

using fmt::format;

namespace tsl {

/**
 * @brief An exception denoting an internal bug.
 */
struct panic_exception : public exception
{
    explicit panic_exception(string msg) : m_msg(std::move(msg)) {}

    const char* what() const noexcept override {
        return m_msg.c_str();
    }

private:
    string m_msg;
};

/**
 * @brief Throws a panic exception with the given error message.
 */
inline void panic(const string& msg)
{
    throw panic_exception("Program panicked: " + msg);
}

/**
 * @brief A version of panic, which behaves like `fmt::format`.
 */
template <typename... Args>
inline void panic(const char* format_str, const Args& ... args) {
    panic(format(format_str, args...));
}

/**
 * @brief Throws a panic exception with the given error message and denotes
 *        that the exception was thrown due to a missing implementation.
 */
inline void panic_unimplemented(const string& msg)
{
    throw panic_exception("Program panicked due to missing implementation: " + msg);
}

/**
 * @brief Unwraps the value in the optional. If there is none, than it panics.
 */
template <typename T>
inline T expect(optional<T> opt, const string& msg)
{
    if (!opt) {
        panic(msg);
    }
    return *opt;
}

}

#endif //TSL_PANIC_HPP
