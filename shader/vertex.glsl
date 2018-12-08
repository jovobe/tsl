#version 330 core
in vec3 aPos;
in vec3 aColor;

out vec3 color;

uniform mat4 MVP;

void main()
{
   gl_Position = MVP * vec4(aPos, 1.0);
   color = aColor;
}
