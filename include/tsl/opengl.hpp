#ifndef TSL_OPENGL_HPP
#define TSL_OPENGL_HPP

#include <GLFW/glfw3.h>

#include <string>

using std::string;

namespace tsl {

GLuint create_program(const string& vertex_shader_path, const string& fragment_shader_path);

}

#endif //TSL_OPENGL_HPP
