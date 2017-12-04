#version 150

uniform samplerCube cube_texture;

in vec3 pass_texcoords;

out vec4 out_Color;

void main() {
  out_Color = texture(cube_texture, pass_texcoords);
}
