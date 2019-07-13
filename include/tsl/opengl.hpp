#ifndef TSL_OPENGL_HPP
#define TSL_OPENGL_HPP

#include <string>
#include <vector>

#include <GL/glew.h>

using std::string;
using std::vector;

namespace tsl {

/**
 * @brief Creates a program with the given shaders.
 */
GLuint create_program(vector<GLuint> shaders);

/**
 * @brief Creates a shader from the given file.
 */
GLuint create_shader(const string& shader_path, GLenum shader_type);

}

#endif //TSL_OPENGL_HPP
