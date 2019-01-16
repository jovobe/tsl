#version 330 core
in vec3 pos;
//in vec3 color_in;

out vec3 color;

uniform mat4 M;
uniform mat4 VP;
uniform vec3 color_in;

void main()
{
   gl_Position = VP * M * vec4(pos, 1.0);
   color = color_in;
}
