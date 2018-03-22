#version 130
precision highp float; // needed only for version 1.30

in vec2 TextureCoord;

uniform vec3 u_color;

out vec4 out_Color;
 
void main(void)
{
    out_Color = vec4(u_color.r, u_color.g, u_color.b, 1.0);
}