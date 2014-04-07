#version 130
precision highp float; // needed only for version 1.30

in vec2 TextureCoord;
uniform sampler2D diffuseTex;

out vec4 out_Color;
 
void main(void)
{
    out_Color = vec4( texture(diffuseTex, TextureCoord).rgb, 1.0) + vec4(0.1, 0.1, 0.1, 1.0);
}