#version 130
precision highp float; // needed only for version 1.30

in vec2 TextureCoord;
uniform sampler2D diffuseTex;

out vec4 out_Color;
 
void main(void)
{
    vec4 texel = vec4( texture(diffuseTex, TextureCoord).rgba);
	if(texel.a == 0.0)
		discard;
    out_Color = texel;
}