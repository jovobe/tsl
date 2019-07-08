#version 330 core
flat in uint picking_id_forward;
in vec3 color;

out uint picking_id;

void main()
{
   picking_id = picking_id_forward;
}
