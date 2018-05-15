#version 130
uniform vec4 offset;

in vec4 in_Position;
in vec4 in_Color;

out vec4 var_color;
 
void main(void)
{
    gl_Position = in_Position + offset;
	var_color = in_Color;
}
