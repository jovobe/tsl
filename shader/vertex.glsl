#version 330 core
in vec3 pos;

out vec3 vcolor;
out uint picking_id;

uniform mat4 M;
uniform vec3 color_in;

void main()
{
   gl_Position = M * vec4(pos, 1.0);
   vcolor = color_in;

   // Pass dummy value
   picking_id = 0u;
}
