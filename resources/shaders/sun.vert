#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 ColorVec3;
uniform int  ShaderMode;

out vec3 pass_Normal;
out vec3 pass_Color;
out vec3 vert_pos_cam;
flat out int shaderMode;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	vert_pos_cam = vec3(ViewMatrix * ModelMatrix * NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	//transform normal otientation
	pass_Normal = vec3(NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	pass_Color = ColorVec3;
	shaderMode = ShaderMode;
}
