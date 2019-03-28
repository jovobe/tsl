#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 vcolor[];
in uint picking_id[];
in uint picked[];

out vec3 color;
flat out uint picking_id_forward;
flat out uint picked_forward;

uniform vec3 camera_pos;
uniform vec3 camera_up;
uniform mat4 VP;

const float POINT_THICKNESS = 0.02f;

void main() {

    // Forward stuff to fragment shader
    color = vcolor[0];
    picking_id_forward = picking_id[0];
    picked_forward = picked[0];

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
}
