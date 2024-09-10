#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in float camera_distance;

uniform sampler2D texture_diffuse1, texture_diffuse2;

const float FOG_DISTANCE = 100;
const vec4 FOG_COLOR     = vec4(104, 83, 126, 255) / 255.f;

void
main()
{
  if (!gl_FrontFacing)
  {
    discard;
  }

  float dist = camera_distance;

  vec2 uv = vec2(1, 1) - TexCoords;

  vec3 col      = vec3(0.3, 0.1, 0.1);
  vec3 lightpos = vec3(100.0, 150.0, 0.0);
  vec3 pointing = normalize(Pos - lightpos);
  float light   = clamp(0.1 + clamp(dot(pointing, Normal), 0.0, 1.0), 0.0, 1.0);

  vec4 textColor1 = ((texture(texture_diffuse1, uv)));
  if (textColor1.a < 0.5) discard;
  // FragColor = textColor1;

  float fog_mask = clamp((1 / (dist / FOG_DISTANCE)), 0, 1);
  textColor1     = clamp(textColor1, vec4(0, 0, 0, 0), textColor1) * fog_mask + vec4(light) * 0.01 * fog_mask;

  vec4 fogColor = FOG_COLOR * (1 - fog_mask);
  fogColor      = clamp(fogColor, vec4(0, 0, 0, 0), vec4(1, 1, 1, 1));

  FragColor = textColor1 + fogColor;
}
