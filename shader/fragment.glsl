#version 330 core
in vec3 color;
flat in uint picked_forward;

out vec4 fragment_color;

const float TINT_FACTOR = 0.5f;

void main()
{
   vec3 color_calculated = color;

   if (picked_forward != 0u) {
      color_calculated = color_calculated + (vec3(1) - color_calculated) * TINT_FACTOR;
      // fragment_color = vec4(1, 1, 1, 1.0f);
   }

   fragment_color = vec4(color_calculated, 1.0f);
}
