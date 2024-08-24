#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Pos;
out float camera_distance;
out vec2 uvs;

// out int InstanceID;

uniform mat4 view;
uniform mat4 projection;
uniform float curr_time;

// Declare the SSBO with the binding point
layout(packed, binding = 1) buffer InstanceBuffer { mat4 instanced_models[]; };

void
main()
{
  mat4 instance_model = instanced_models[gl_InstanceID];
  vec3 world_position = vec3(instance_model[3][0], instance_model[3][1], instance_model[3][2]);

  vec3 pPos = aPos;

  vec3 camera_pos = vec3(view[0][2], view[1][2], view[2][2]);
  camera_distance = length(view * instance_model * vec4(pPos, 1.0) - vec4(camera_pos, 0));

  pPos.x += cos(curr_time * 1.5 + length(world_position + aPos * 0.5) * max(aPos.y, 0)) * 0.08 * max(aPos.y, 0);
  pPos.z += sin(curr_time * 1.5 + length(world_position + aPos * 0.5) * max(aPos.y, 0)) * 0.08 * max(aPos.y, 0);

  TexCoords   = aTexCoords;
  Normal      = normalize(mat3(transpose(inverse(instance_model))) * aNormal);
  gl_Position = projection * view * instance_model * vec4(pPos, 1.0);
  uvs         = aTexCoords;
}