#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out vec2 uvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
  TexCoords   = aTexCoords;
  Normal      = normalize(mat3(transpose(inverse(model))) * aNormal);
  Pos         = aPos;
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  uvs         = aTexCoords;
}