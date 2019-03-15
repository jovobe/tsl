#ifndef TSL_OPENGL_HPP
#define TSL_OPENGL_HPP

#include <GL/glew.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace tsl {

GLuint create_program(vector<GLuint> shaders);

GLuint create_shader(const string& shader_path, GLenum shader_type);

}

#endif //TSL_OPENGL_HPP
