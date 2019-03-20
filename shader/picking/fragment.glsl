#version 330 core
in vec3 color;

//out vec3 fragment_color;
out uint fragment_color;

void main()
{
//   fragment_color = vec4(color.x, 0.0f, 0.0f, 1.0f);
//   fragment_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
//   fragment_color = uint(42);
   fragment_color = uint(color.x);
//   fragment_color = vec3(color.x, gl_PrimitiveID + 1, 1.0);
}
