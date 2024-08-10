#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;

uniform sampler2D texture_diffuse;

void
main()
{
  vec4 col = texture(texture_diffuse, vec2(1, 1) - TexCoords);
  if (col.a <= 0.5) discard;
  FragColor = col;
}