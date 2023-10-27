#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;

uniform sampler2D texture_diffuse1;

void
main()
{
  if (!gl_FrontFacing)
  {
    discard;
  }

  vec2 uv       = TexCoords;
  vec3 col      = 0.5 + 0.5 * cos(uv.xyx + vec3(0, 2, 4));
  vec3 lightpos = vec3(100.0, 50.0, 600.0);
  vec3 pointing = normalize(Pos - lightpos);

  float light = clamp(0.1 + clamp(dot(pointing, Normal), 0.0, 1.0), 0.0, 1.0);

  FragColor = texture(texture_diffuse1, TexCoords) * light * vec4(1.0, 0.9 + 0.2, 0.9, 1.0);
}
