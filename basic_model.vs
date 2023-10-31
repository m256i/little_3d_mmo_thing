#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out float camera_distance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
  vec3 camera_pos = vec3(view[0][2], view[1][2], view[2][2]);
  camera_distance = length(projection * view * model * vec4(aPos, 1.0) - vec4(camera_pos, 0));

  TexCoords   = aTexCoords;
  Normal      = aNormal;
  Pos         = aPos;
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}