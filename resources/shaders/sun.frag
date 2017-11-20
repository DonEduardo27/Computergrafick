#version 150

in vec3 pass_Normal;
in vec3 pass_Color;
in vec3 vert_pos;
in vec3 vert_pos_world;
out vec4 out_Color;

void main() {
  vec3 normal = normalize(pass_Normal);
  out_Color = vec4(pass_Color, 1.0);
}
