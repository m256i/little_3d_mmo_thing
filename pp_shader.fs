#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture_diffuse;

void
main()
{
  FragColor = vec4(texture(texture_diffuse, TexCoords).rgb, 1.0);
}