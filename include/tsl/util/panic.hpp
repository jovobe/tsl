#ifndef TSL_PANIC_HPP
#define TSL_PANIC_HPP

#include <utility>
#include <exception>
#include <string>

using std::move;
using std::exception;
using std::string;

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
 * @brief Throws a panic exception with the given error message and denotes
 *        that the exception was thrown due to a missing implementation.
 */
inline void panic_unimplemented(const string& msg)
{
    throw panic_exception("Program panicked due to missing implementation: " + msg);
}

}

#endif //TSL_PANIC_HPP
