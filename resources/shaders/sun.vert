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
//uniform vec3 Color;

out vec3 pass_Normal;
out vec3 vert_pos;
out vec3 cam_pos;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	//inverse Matrix Extraction
	mat4 viewModel = inverse(ViewMatrix);
	vec3 cam_pos = viewModel[3].xyz;

	//transform vertex position
	vec4 vertPos4 = ModelMatrix * vec4(in_Position, 1.0);
	vert_pos = vec3(vertPos4) / vertPos4.w;

	//transform normal otientation
	pass_Normal = vec3(NormalMatrix * vec4(in_Normal, 0.0));

}
