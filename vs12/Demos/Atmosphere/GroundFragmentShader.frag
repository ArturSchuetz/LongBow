#version 130
precision highp float; // needed only for version 1.30

in vec2 TextureCoord;
in vec3 Color;
in vec3 Attenuate;

uniform sampler2D diffuseTex;

out vec3 out_Color;
 
void main(void)
{
	vec3 texel = texture(diffuseTex, TextureCoord).rgb;
	//out_Color = texel;
    out_Color = Color + texel * Attenuate;
}