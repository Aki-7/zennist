#version 320 es

uniform mat4 zVP;
uniform mat4 zModel;

uniform mat4 local_model;
uniform mat4 base_model;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 norm;
layout(location = 2) in vec2 texcoord;

out vec3 in_position;
out vec3 in_normal;
out vec2 in_uv;

void
main()
{
  mat4 model = zModel * base_model * local_model;
  gl_Position = zVP * model * position;
  in_position = (model * position).xyz;
  in_normal = normalize((model * norm - model * vec4(0.0, 0.0, 0.0, 1.0)).xyz);
  in_uv = texcoord;
}
