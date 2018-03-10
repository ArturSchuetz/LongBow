#version 140

in vec3 in_Position;
in vec3 in_Normal;

out vec3 var_Color;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0) * u_ModelView * u_Proj; // multiplying a vector from the left to a matrix corresponds to multiplying it from the right to the transposed matrix
    
	vec3 normal = normalize(vec4(vec4(in_Normal, 0.0) * u_ModelView).xyz);
	vec3 lightDirection = normalize(vec4(vec4(1.0, 1.0, -1.0, 0.0) * u_ModelView).xyz);

	float intensity = max(dot(normal, lightDirection), 0.0);

	var_Color = intensity * vec3(1.0, 1.0, 1.0) + vec3(0.1, 0.1, 0.1);
}