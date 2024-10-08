#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;

out float camera_distance;
out vec2 uvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
  vec3 camera_pos = vec3(view[0][2], view[1][2], view[2][2]);
  camera_distance = length(view * model * vec4(position, 1.0) - vec4(camera_pos, 0));

  TexCoords   = tex_coords;
  Normal      = normalize(mat3(transpose(inverse(model))) * normal);
  Pos         = position;
  gl_Position = projection * view * model * vec4(position, 1.0);
  uvs         = tex_coords;
}