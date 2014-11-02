#version 130
 
precision highp float; // needed only for version 1.30

uniform vec4 u_color;
out vec4 out_Color;
 
void main(void)
{
    out_Color = u_color;
}