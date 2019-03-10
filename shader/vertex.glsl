#version 330 core
in vec3 pos;
//in vec3 color_in;

//out vec3 vcolor;

uniform mat4 M;
//uniform mat4 VP;
//uniform vec3 color_in;

void main()
{
   gl_Position = M * vec4(pos, 1.0);
//   vcolor = color_in;
}
