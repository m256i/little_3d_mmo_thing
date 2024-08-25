#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in float camera_distance;

uniform sampler2D texture_diffuse1;

void
main()
{
  if (!gl_FrontFacing)
  {
    discard;
  }

  vec4 textColor = (texture(texture_diffuse1, TexCoords);

  uint funny = 0;
  FragColor = textColor + fogColor;
}
