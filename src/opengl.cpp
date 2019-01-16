#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <tsl/opengl.hpp>
#include <tsl/read_file.hpp>

using std::cout;
using std::endl;
using std::vector;

namespace tsl {

GLuint create_program(const string& vertex_shader_path, const string& fragment_shader_path) {

    // Vertex shader
    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto vertex_shader_text = read_file(vertex_shader_path);
//    cout << vertex_shader_text << endl;
    // TODO: why variable?
    auto tmp1 = vertex_shader_text.c_str();
    glShaderSource(vertex_shader, 1, &tmp1, nullptr);
    glCompileShader(vertex_shader);
    GLint res;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &res);
    cout << "vertex shader '" << vertex_shader_path << "' compile status: " << res << endl;
    if (res == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

        vector<GLchar> error_log(static_cast<unsigned long>(max_length));
        glGetShaderInfoLog(vertex_shader, max_length, &max_length, &error_log[0]);
        string log(error_log.begin(), error_log.end());

        // pop null terminator
        log.pop_back();

        cout << "Compiling shader failed:\n" << log << endl;

        glDeleteShader(vertex_shader); // Don't leak the shader.
        exit(1);
    }

    // Fragment shader
    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragment_shader_text = read_file(fragment_shader_path);
    tmp1 = fragment_shader_text.c_str();
    glShaderSource(fragment_shader, 1, &tmp1, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &res);
    cout << "fragment '" << fragment_shader_path << "' compile status: " << res << endl;
    if (res == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

        vector<GLchar> error_log(static_cast<unsigned long>(max_length));
        glGetShaderInfoLog(fragment_shader, max_length, &max_length, &error_log[0]);
        string log(error_log.begin(), error_log.end());

        // pop null terminator
        log.pop_back();

        cout << "Compiling shader failed:\n" << log << endl;

        glDeleteShader(fragment_shader); // Don't leak the shader.
        exit(1);
    }

    auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    cout << "program link status: " << is_linked << endl;
    if (is_linked == GL_FALSE)
    {
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        std::vector<GLchar> info_log(static_cast<unsigned long>(max_length));
        glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        string log(info_log.begin(), info_log.end());

        // pop null terminator
        log.pop_back();

        cout << "Linking program failed:\n" << log << endl;

        // In this simple program, we'll just leave
        exit(1);
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

}
