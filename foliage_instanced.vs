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
uniform float animtime;

// Declare the SSBO with the binding point
layout(packed, binding = 1) buffer InstanceBuffer { mat4 instanced_models[]; };

void
main()
{
  mat4 instance_model = instanced_models[gl_InstanceID];

  vec3 world_position = vec3(instance_model[3][0], instance_model[3][1], instance_model[3][2]);
  Pos                 = aPos + world_position;

  vec3 camera_pos = vec3(view[0][2], view[1][2], view[2][2]);
  camera_distance = length(view * instance_model * vec4(Pos, 1.0) - vec4(camera_pos, 0));

  vec4 foliage_animation = vec4(cos(animtime), 0, 0, 0) + vec4(0, sin(animtime), 0, 0);

  TexCoords   = aTexCoords;
  Normal      = normalize(mat3(transpose(inverse(instance_model))) * aNormal);
  gl_Position = projection * view * instance_model * (vec4(Pos, 1.0) + foliage_animation);
  uvs         = aTexCoords;
}