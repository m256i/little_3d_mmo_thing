#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float camera_distance;

void
main()
{
  vec3 camera_pos = vec3(view[0][2], view[1][2], view[2][2]);
  camera_distance = length(view * model * vec4(aPos, 1.0) - vec4(camera_pos, 0));

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
