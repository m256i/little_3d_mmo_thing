#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 TexCoords;

// out float camera_distance;
out vec2 uvs;
out int instance_id;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  // uvs         = TexCoords.xy;
  // instance_id = gl_InstanceID;
}