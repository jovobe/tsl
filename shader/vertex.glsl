#version 330 core
in vec3 pos;
in vec3 color_in;

//out vec3 color;

uniform mat4 MVP;

void main()
{
   gl_Position = MVP * vec4(pos, 1.0);
//   color = color_in;
}
