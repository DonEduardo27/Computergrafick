#version 150 core
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec2 in_Position;

out vec2 pass_texcoord;

void main(void){
    pass_texcoord = in_Position;
    gl_Position = vec4(in_Position, 0.0, 1.0);
}
