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

out vec3 pass_Normal;
out vec3 vert_pos;
out mat4 View_Matrix;
//out vec3 camera_pos;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	//inverse Matrix Extraction
	//mat4 viewModel = inverse(ViewMatrix);
	//camera_pos = viewModel[3].xyz;

	//transform vertex position
	vec4 vertPos4 = ModelMatrix * vec4(in_Position, 1.0);
	vert_pos = vec3(vertPos4) / vertPos4.w;

	View_Matrix = ViewMatrix;

	//transform normal otientation
	pass_Normal = vec3(NormalMatrix * vec4(in_Normal, 0.0)).xyz;

}
