#version 150

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_TexCoord;


uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

out vec3 pass_TexCoord;

void main(void) {
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(in_Position, 1.0);
  pass_TexCoord = in_TexCoord;
}
