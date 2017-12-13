#version 150 core

in vec2 pass_texcoord;
uniform sampler2D texFramebuffer;

out vec4 outColor;

void main() {
    outColor = texture(texFramebuffer, pass_texcoord);
}
