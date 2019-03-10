#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
//layout (points, max_vertices = 1) out;

out vec3 color;

uniform vec3 color_in;
uniform vec3 camera_pos;
uniform vec3 camera_up;
uniform mat4 VP;

const float POINT_THICKNESS = 0.02f;

void main() {

    // Forward color
    color = color_in;

    vec3 point = gl_in[0].gl_Position.xyz;
    vec3 point_to_camera = normalize(camera_pos - point);
    vec3 left_right_offset_direction = normalize(cross(point_to_camera, camera_up));
    vec3 up_down_offset_direction = normalize(cross(point_to_camera, left_right_offset_direction));

    gl_Position = VP * (gl_in[0].gl_Position + vec4(up_down_offset_direction * POINT_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (gl_in[0].gl_Position + vec4(left_right_offset_direction * POINT_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (gl_in[0].gl_Position - vec4(left_right_offset_direction * POINT_THICKNESS, 0.0));
    EmitVertex();

    gl_Position = VP * (gl_in[0].gl_Position - vec4(up_down_offset_direction * POINT_THICKNESS, 0.0));
    EmitVertex();

    EndPrimitive();

//    gl_Position = VP * gl_in[0].gl_Position;
//    EmitVertex();
//    EndPrimitive();
}
