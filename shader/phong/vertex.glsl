#version 330 core
in vec3 pos_in;
in vec3 normal_in;
in uint picked_in;

out vec3 color;
out vec3 normal;
out vec3 pos;
flat out uint picked;

uniform mat4 M;
uniform mat4 VP;
uniform vec3 color_in;

void main()
{
   gl_Position = VP * M * vec4(pos_in, 1.0);
   color = color_in;
   picked = picked_in;
   normal = (M * vec4(normal_in, 0)).xyz;
   pos = (M * vec4(pos_in, 1.0)).xyz;
}
