#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 TexCoords;

// out float camera_distance;
out vec2 uvs;

out int instance_id;

void
main()
{
  // vec3 camera_pos = vec3(view[0][2], view[1][2], view[2][2]);
  // camera_distance = length(view * model * vec4(aPos, 1.0) - vec4(camera_pos, 0));

  gl_Position = vec4(aPos + vec3(gl_InstanceID, gl_InstanceID, 0), 1.0);
  uvs         = TexCoords.xy;
  instance_id = gl_InstanceID;
}
