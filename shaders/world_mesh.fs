#version 430 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in float camera_distance;

uniform sampler2D texture_diffuse1, texture_diffuse2;

const float FOG_DISTANCE = 600;
const vec4 FOG_COLOR     = vec4(104, 83, 126, 255) / 255.f;

const vec3 SUN_COLOR     = vec3(0.65, 0.3, 0.01);
const float SUN_STRENGTH = 0.2;

void
main()
{
  if (!gl_FrontFacing)
  {
    discard;
  }

  float dist = camera_distance;

  highp vec2 uv  = vec2(Pos.x, Pos.z);
  highp vec2 uv2 = vec2(Pos.y, Pos.z);

  uv  = mod(uv * 50 + 0.001, vec2(1.0, 1.0));
  uv2 = mod(uv2 * 50 + 0.001, vec2(1.0, 1.0));

  vec3 col      = SUN_COLOR * SUN_STRENGTH;
  vec3 lightpos = vec3(100.0, -100.0, 200.0);
  vec3 pointing = normalize(Pos - lightpos);
  float light   = clamp(0.1 + clamp(dot(pointing, Normal), 0.0, 1.0), 0.0, 1.0) - 0.5;

  float fog_mask = clamp((1 / (dist / FOG_DISTANCE)), 0, 1);

  vec4 textColor1 = ((texture(texture_diffuse1, uv)) * Normal.y);

  textColor1 = clamp(textColor1, vec4(0, 0, 0, 0), textColor1) * fog_mask + vec4(light * col, 1) * fog_mask;

  vec4 textColor2 = ((texture(texture_diffuse2, uv2)) * (1 - Normal.y));
  textColor2      = clamp(textColor2, vec4(0, 0, 0, 0), textColor2) * fog_mask + vec4(light * col, 1) * fog_mask;

  vec4 fogColor = FOG_COLOR * (1 - fog_mask);
  fogColor      = clamp(fogColor, vec4(0, 0, 0, 1), vec4(1, 1, 1, 1));

  FragColor = textColor1 + textColor2 + fogColor;
}
