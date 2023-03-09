#version 320 es

precision mediump float;

uniform mat4 zView;
uniform sampler2D in_texture;

uniform vec2 in_offset;
uniform vec2 in_scale;
uniform float in_rotation;

uniform float in_alpha_cutoff;

in vec3 in_position;
in vec3 in_normal;
in vec2 in_uv;

out vec4 outputColor;

const float Ka = 0.8;
const float Kd = 0.8;
const float Ks = 0.5;
const float alpha = 32.0;

void
main()
{
  mat3 translation = mat3(1,0,0, 0,1,0, in_offset.x, in_offset.y, 1);
  mat3 rotation = mat3(
      cos(in_rotation), sin(in_rotation), 0,
     -sin(in_rotation), cos(in_rotation), 0,
                  0,             0, 1
  );
  mat3 scale = mat3(in_scale.x,0,0, 0,in_scale.y,0, 0,0,1);
  mat3 matrix = translation * rotation * scale;
  vec2 uv_transformed = ( matrix * vec3(in_uv.xy, 1) ).xy;

  vec4 color = texture(in_texture, uv_transformed);
  if (color.a < in_alpha_cutoff) {
    discard;
  }

  vec4 camera_position = inverse(zView) * vec4(0.0, 0.0, 0.0, 1.0);
  vec3 light_position = vec3(1.0, 4.0, 0.0);

  vec3 view = normalize(camera_position.xyz - in_position);
  vec3 light = normalize(light_position - in_position);
  vec3 half_way = normalize(light + view);

  float ambient = Ka;
  float diffuse = Kd * max(dot(light, in_normal), 0.0);
  float specular = Ks * pow(max(dot(half_way, in_normal), 0.0), alpha);

  outputColor = vec4(color.xyz * (ambient + diffuse) + vec3(1.0, 1.0, 1.0) * specular, 1.0) * 0.3;
}
