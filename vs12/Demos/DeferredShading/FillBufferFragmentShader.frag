#version 330
precision highp float; // needed only for version 1.30

in vec2 TextureCoord;
in vec3 Normal;

uniform sampler2D diffuseTex;

out vec3 out_Albedo;
out vec3 out_Normal;
 
void main(void)
{
	vec4 texel = vec4( texture(diffuseTex, TextureCoord).rgba);
	if(texel.a < 1.0)
		discard;

	out_Albedo = texel.rgb;
	out_Normal = Normal;
}
/*