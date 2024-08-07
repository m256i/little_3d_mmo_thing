#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in float camera_distance;

uniform sampler2D texture_diffuse1, texture_diffuse2;

const float FOG_DISTANCE = 500;
const vec4 FOG_COLOR     = vec4(97 / 255.f, 97 / 255.f, 112 / 255.f, 1.f);

void
main()
{
  if (!gl_FrontFacing)
  {
    discard;
  }
  float dist = camera_distance;

  vec2 uv  = vec2(Pos.x * 0.75, Pos.y * 0.75);
  vec2 uv2 = vec2(Pos.y * 0.75, Pos.z * 0.75);

  vec3 col      = 0.5 + 0.5 * cos(uv.xyx + vec3(0, 2, 4));
  vec3 lightpos = vec3(100.0, 150.0, 0.0);
  vec3 pointing = normalize(Pos - lightpos);
  float light   = clamp(0.1 + clamp(dot(pointing, Normal), 0.0, 1.0), 0.0, 1.0);

  float fog_mask = (1 / (dist / FOG_DISTANCE));

  vec4 textColor1 = ((texture(texture_diffuse1, uv)) * Normal.y);
  textColor1      = clamp(textColor1 * fog_mask, vec4(0, 0, 0, 1), textColor1);

  vec4 textColor2 = ((texture(texture_diffuse2, uv2)) * (1 - Normal.y));
  textColor2      = clamp(textColor2 * fog_mask, vec4(0, 0, 0, 1), textColor2);

  vec4 fogColor = FOG_COLOR * (1 - fog_mask);
  fogColor      = clamp(fogColor, vec4(0, 0, 0, 1), vec4(1, 1, 1, 1));

  FragColor = textColor1 + textColor2 + fogColor;
}
