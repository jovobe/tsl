#version 330 core
in vec3 pos;
in uint picking_id_in;

flat out uint picking_id_forward;
out vec3 color;

uniform mat4 M;
uniform mat4 VP;

void main()
{
   gl_Position = VP * M * vec4(pos, 1.0);
   picking_id_forward = picking_id_in;

   // Pass dummy value
   color = vec3(0, 0, 0);
}
