#version 130
precision highp float; // needed only for version 1.30

in vec2 TextureCoord;

out vec4 out_Color;
 
void main(void)
{
    out_Color = vec4(0.0, 0.0, 0.0, 1.0);
}