#version 140
precision highp float; // needed only for version 1.30

in vec2 TextureCoord;
uniform sampler2D diffuseTex;

out vec3 out_Color;
 
void main(void)
{
    out_Color = vec3(texture(diffuseTex, TextureCoord).rgb);
}