#version 150 core
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_TexCood;

uniform vec4 EffectMode;

out vec2 pass_texcoord;
out vec4 effectMode;

void main(void){
    effectMode = EffectMode;
    pass_texcoord = in_TexCood;
    gl_Position = vec4(in_Position, 0.0, 1.0);
}
