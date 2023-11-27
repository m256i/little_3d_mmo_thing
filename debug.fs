#version 430 core

// in float camera_distance;

out vec4 FragColor;
uniform vec4 in_color;

in vec3 out_p;
in float camera_distance;

const float FOG_DISTANCE = 70;
const vec4 FOG_COLOR     = vec4(55.f / 255.f, 55.f / 255.f, 182.f / 255.f, 1);

void
main()
{
  // FragColor = vec4(in_color.xy * ((1 / (camera_distance)*6) + 0.3), in_color.zw);

  float dist     = camera_distance;
  float fog_mask = (1 / (dist / FOG_DISTANCE));
  vec4 base_col  = vec4((out_p.z) * 89.f / 255.f, (out_p.z) * 138.f / 255.f, (out_p.z) * 47 / 255.f, 1);
  vec4 textColor = (base_col);
  textColor      = clamp(textColor * fog_mask, vec4(0, 0, 0, 1), textColor);

  vec4 fogColor = FOG_COLOR * (1 - fog_mask);
  fogColor      = clamp(fogColor, vec4(0, 0, 0, 1), vec4(1, 1, 1, 1));

  FragColor = textColor + fogColor;
}