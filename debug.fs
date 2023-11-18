#version 330 core

in float camera_distance;

out vec4 FragColor;

uniform vec4 in_color;

void
main()
{
  FragColor = vec4(in_color.xy * ((1 / (camera_distance)*6) + 0.3), in_color.zw);
}
