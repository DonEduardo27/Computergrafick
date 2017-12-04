#version 150

uniform samplerCube cube_texture;
smooth in vec3 eyeDirection;

out vec4 out_Color;

void main() {
    out_Color = texture(cube_texture, eyeDirection);
}
