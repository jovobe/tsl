#version 330 core
in vec3 pos_in;
in vec3 normal_in;

out vec3 vcolor;
out vec4 normal;

uniform mat4 M;
uniform vec3 color_in;

void main()
{
    gl_Position = M * vec4(pos_in, 1.0);
    vcolor = color_in;
    normal = M * vec4(normal_in, 0);
}
