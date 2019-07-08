#version 330 core
in vec3 pos;
in uint picking_id_in;

out vec3 vcolor;
out uint picking_id;
out uint picked;

uniform mat4 M;

void main()
{
   gl_Position = M * vec4(pos, 1.0);
   picking_id = picking_id_in;

   // Pass dummy value
   vcolor = vec3(0, 0, 0);
   picked = 0u;
}
