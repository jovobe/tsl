#ifndef TSE_OPENGL_HPP
#define TSE_OPENGL_HPP

#include <string>
#include <vector>

#include <GL/glew.h>

using std::string;
using std::vector;

namespace tse {

/**
 * @brief Creates a program with the given shaders.
 */
GLuint create_program(vector<GLuint> shaders);

/**
 * @brief Creates a shader from the given file.
 */
GLuint create_shader(const string& shader_path, GLenum shader_type);

}

#endif //TSE_OPENGL_HPP
