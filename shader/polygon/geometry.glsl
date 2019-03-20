#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 vcolor[];

out vec3 color;

uniform vec3 camera_pos;
uniform mat4 VP;

const float LINE_THICKNESS = 0.01f;

vec3 get_offset_direction(const in vec3 point1, const in vec3 point2) {
    vec3 point1_to_camera = normalize(camera_pos - point1);
    vec3 point1_to_point2 = normalize(point2 - point1);
    return normalize(cross(point1_to_point2, point1_to_camera));
}

void main() {

    // Forward color
    color = vcolor[0];

    vec3 point1 = gl_in[0].gl_Position.xyz;
    vec3 point2 = gl_in[1].gl_Position.xyz;

    vec3 offset_direction_1 = get_offset_direction(point1, point2);
    vec3 offset_direction_2 = get_offset_direction(point2, point1);

    gl_Position = VP * (gl_in[0].gl_Position + vec4(offset_direction_1 * LINE_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (gl_in[0].gl_Position - vec4(offset_direction_1 * LINE_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (gl_in[1].gl_Position - vec4(offset_direction_2 * LINE_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (gl_in[1].gl_Position + vec4(offset_direction_2 * LINE_THICKNESS, 0.0));
    EmitVertex();

    EndPrimitive();
}
