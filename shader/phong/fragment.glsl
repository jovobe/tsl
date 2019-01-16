#version 330 core
in vec3 color;
in vec3 normal;
in vec3 pos;

out vec4 fragment_color;

uniform vec3 camera_pos;

vec3 light_pos = vec3(0, 3, 3);
vec3 light_color = vec3(1);
float light_power = 1.0;
float specular_power = 0.25;
vec3 ambient_light = vec3(0.1);

void main()
{
    vec3 inverse_light_direction = normalize(light_pos - pos);
    vec3 inverse_camera_direction = normalize(camera_pos - pos);
    vec3 reflected_light_direction = reflect(-inverse_light_direction, normal);

    float angle_light_direction = clamp(dot(inverse_light_direction, normalize(normal)), 0, 1);
    float specular_factor = clamp(dot(inverse_camera_direction, reflected_light_direction), 0, 1);

    vec3 ambient_color = color * ambient_light;
    vec3 diffuse_color = light_power * light_color * color * angle_light_direction;
    vec3 specular_color = specular_power * light_power * light_color * pow(specular_factor, 5);

//   fragment_color = vec4(diffuse_color + ambient_color + specular_color, 1.0f);
   fragment_color = vec4(diffuse_color + ambient_color, 1.0f);
}
