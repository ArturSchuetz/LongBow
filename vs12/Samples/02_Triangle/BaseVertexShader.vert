#version 130

in vec4 in_Position;
in vec4 in_Color;

out vec4 var_color;
 
void main(void)
{
    gl_Position = in_Position;
	var_color = in_Color;
}
