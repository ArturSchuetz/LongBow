#version 140
precision highp float; // needed only for version 1.30

in vec4 in_Position;
in vec3 in_Normal;

out vec3 out_Position;
out vec3 out_Normal;
out float out_Color;

uniform mat4 u_ModelViewProj;
uniform mat4 u_ModelView;

void main(void)
{	
    gl_Position = vec4(in_Position.xyz, 1.0) * u_ModelViewProj; // multiplying a vector from the left to a matrix corresponds to multiplying it from the right to the transposed matrix
    
	out_Normal = (vec4(in_Normal, 0.0) * u_ModelView).xyz;
	out_Position = in_Position.xyz;

	out_Color = in_Position.w;
}





























