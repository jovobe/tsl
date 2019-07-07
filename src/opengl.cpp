#include <vector>

#include <GL/glew.h>

#include "tsl/opengl.hpp"
#include "tsl/read_file.hpp"
#include "tsl/util/println.hpp"

using std::vector;

namespace tsl {

GLuint create_program(vector<GLuint> shaders) {
    auto program = glCreateProgram();
    for (const auto& shader: shaders) {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    println("program link status: {}", is_linked);
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
        for (const auto& shader: shaders) {
            glDeleteShader(shader);
        }

        string log(info_log.begin(), info_log.end());

        // pop null terminator
        log.pop_back();

        println("Linking program failed:\n{}", log);

        // In this simple program, we'll just leave
        exit(1);
    }

    // Always detach shaders after a successful link.
    for (const auto& shader: shaders) {
        glDetachShader(program, shader);
    }

    return program;
}

GLuint create_shader(const string& shader_path, GLenum shader_type) {
    auto shader = glCreateShader(shader_type);
    auto shader_text = read_file(shader_path);
//    println(shader_text.c_str());
    // TODO: why variable?
    auto tmp1 = shader_text.c_str();
    glShaderSource(shader, 1, &tmp1, nullptr);
    glCompileShader(shader);
    GLint res;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
    println("shader '{}' compile status: {}", shader_path, res);
    if (res == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        vector<GLchar> error_log(static_cast<unsigned long>(max_length));
        glGetShaderInfoLog(shader, max_length, &max_length, &error_log[0]);
        string log(error_log.begin(), error_log.end());

        // pop null terminator
        log.pop_back();

        println("Compiling shader failed:\n{}", log);

        glDeleteShader(shader); // Don't leak the shader.
        exit(1);
    }

    return shader;
}

}
