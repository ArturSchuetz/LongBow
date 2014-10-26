#version 140

in vec3 in_Position;

uniform mat4 u_ModelViewProj;

void main(void)
{
    gl_Position = u_ModelViewProj * vec4(in_Position, 1.0);
}