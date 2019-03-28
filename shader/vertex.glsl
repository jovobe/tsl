#version 330 core
in vec3 pos;
in uint picked_in;

out vec3 vcolor;
out uint picking_id;
out uint picked;

uniform mat4 M;
uniform vec3 color_in;

void main()
{
   gl_Position = M * vec4(pos, 1.0);
   vcolor = color_in;
   picked = picked_in;

   // Pass dummy value
   picking_id = 0u;
}
