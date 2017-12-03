#version 150

in vec3 pass_TexCoord;
out vec4 out_Color;

uniform sampleCube cubeMap;

void main(void) {
  out_Color = texture(cubeMap, pass_TexCoord);
}
