#version 330 core
in vec3 pos;
in vec3 picking_id;

out vec3 vcolor;

uniform mat4 M;

void main()
{
   gl_Position = M * vec4(pos, 1.0);
   vcolor = picking_id;
}
