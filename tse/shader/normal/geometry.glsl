#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 vcolor[];
in vec4 normal[];

out vec3 color;

uniform vec3 camera_pos;
uniform mat4 VP;

const float LINE_THICKNESS = 0.001f;
const float NORMAL_LENGTH = 0.25f;

vec3 get_offset_direction(const in vec3 point1, const in vec3 point2) {
    vec3 point1_to_camera = normalize(camera_pos - point1);
    vec3 point1_to_point2 = normalize(point2 - point1);
    return normalize(cross(point1_to_point2, point1_to_camera));
}

void main() {

    // Forward color to fragment shader
    color = vcolor[0];

    vec3 point1 = gl_in[0].gl_Position.xyz;
    vec3 point2 = point1 + normal[0].xyz * NORMAL_LENGTH;

    vec3 offset_direction_1 = get_offset_direction(point1, point2);
    vec3 offset_direction_2 = get_offset_direction(point2, point1);

    vec4 pos1 = gl_in[0].gl_Position;
    vec4 pos2 = pos1 + normal[0] * NORMAL_LENGTH;

    gl_Position = VP * (pos1 + vec4(offset_direction_1 * LINE_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (pos1 - vec4(offset_direction_1 * LINE_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (pos2 - vec4(offset_direction_2 * LINE_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (pos2 + vec4(offset_direction_2 * LINE_THICKNESS, 0.0));
    EmitVertex();

    EndPrimitive();
}
