#version 140

in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec3 in_Bitangent;
in vec2 in_TexCoord;

out vec3 var_lightDir;
out vec2 var_TexCoord;

uniform mat4 u_ModelView;
uniform mat4 u_View;
uniform mat4 u_Proj;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0) * u_ModelView * u_Proj; // multiplying a vector from the left to a matrix corresponds to multiplying it from the right to the transposed matrix
	
    vec3 Tangent = vec4(vec4(in_Tangent.xyz, 0.0) * u_ModelView).xyz;
    vec3 Bitangent = vec4(vec4(in_Bitangent.xyz, 0.0) * u_ModelView).xyz;
	vec3 Normal = vec4(vec4(in_Normal.xyz, 0.0) * u_ModelView).xyz;
	
	mat3 tangentMatrix = mat3(Tangent, Bitangent, Normal);

	vec3 light_position = vec4(vec4(500.0, 1000.0, 500.0, 0.0) * u_View).xyz;

	var_lightDir = normalize(light_position) * tangentMatrix;
	var_TexCoord = in_TexCoord;
}
