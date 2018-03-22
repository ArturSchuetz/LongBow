#version 130

in vec3 in_Position;
out vec3 out_Position; 

void main(void)
{
	out_Position = in_Position;
    gl_Position = vec4(in_Position, 1.0);
}