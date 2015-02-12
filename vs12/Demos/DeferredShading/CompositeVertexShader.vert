#version 330

in vec3 in_Position;

uniform mat4 u_ViewProj;
uniform vec3 u_lightPos;
uniform float u_radius;

void main(void)
{
	vec4 position = vec4(in_Position, 1.0);
	position.xyz *= u_radius;
	position.xyz += u_lightPos;
	position.w = 1.0;
	gl_Position = position * u_ViewProj;

}
/*