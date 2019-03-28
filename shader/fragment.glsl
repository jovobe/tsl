#version 330 core
in vec3 color;
flat in uint picked_forward;

out vec4 fragment_color;

void main()
{
   if (picked_forward == 0u) {
      fragment_color = vec4(color, 1.0f);
   } else {
      fragment_color = vec4(1, 1, 1, 1.0f);
   }

//   fragment_color = vec4(0, 1.0f, 0, 1.0f);
}
