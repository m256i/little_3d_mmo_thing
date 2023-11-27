#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in float camera_distance;

uniform sampler2D texture_diffuse1;

const float FOG_DISTANCE = 100;
const vec4 FOG_COLOR     = vec4(50 / 255.f, 42 / 255.f, 43 / 255.f, 1.f);

void
main()
{
  if (!gl_FrontFacing)
  {
    discard;
  }
  float dist = camera_distance;

  vec2 uv       = TexCoords;
  vec3 col      = 0.5 + 0.5 * cos(uv.xyx + vec3(0, 2, 4));
  vec3 lightpos = vec3(100.0, 150.0, 0.0);
  vec3 pointing = normalize(Pos - lightpos);
  float light   = clamp(0.1 + clamp(dot(pointing, Normal), 0.0, 1.0), 0.0, 1.0);

  float fog_mask = (1 / (dist / FOG_DISTANCE));

  vec4 textColor = (texture(texture_diffuse1, TexCoords) * light * vec4(1.0, 1.0, 0.9, 1.0));
  textColor      = clamp(textColor * fog_mask, vec4(0, 0, 0, 1), textColor);

  vec4 fogColor = FOG_COLOR * (1 - fog_mask);
  fogColor      = clamp(fogColor, vec4(0, 0, 0, 1), vec4(1, 1, 1, 1));

  FragColor = textColor + fogColor;
}
