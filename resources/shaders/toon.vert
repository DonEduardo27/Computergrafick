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

out vec3 pass_Normal;
out vec3 pass_Color;
out vec3 vert_pos;
out vec3 vert_pos_world;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);

	vec4 vertPos4 = ModelMatrix * vec4(in_Position, 1.0);
	vert_pos = vec3(vertPos4) / vertPos4.w;
	//position independent from Camera rotation:
	vert_pos_world = vec3(ViewMatrix * vec4(vert_pos,0.0) ).xyz;
	//transform normal otientation
	pass_Normal = vec3(NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	pass_Color = ColorVec3;
}
