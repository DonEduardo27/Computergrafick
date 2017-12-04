#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_TexCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
out vec3 pass_texcoords;

void main() {
  pass_texcoords = in_TexCoord;
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(in_TexCoord, 1.0);
}
