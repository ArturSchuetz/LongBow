#version 140

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;

out vec3 var_LightDir;
out vec3 var_Normal;
out vec2 var_TexCoord;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0) * u_ModelView * u_Proj; // multiplying a vector from the left to a matrix corresponds to multiplying it from the right to the transposed matrix
	var_LightDir = normalize(vec4(vec4(500.0, 800.0, 2000.0, 0.0) * u_ModelView).xyz);
	var_Normal = normalize(vec4(vec4(in_Normal, 0.0) * u_ModelView).xyz);
	var_TexCoord = in_TexCoord;
}