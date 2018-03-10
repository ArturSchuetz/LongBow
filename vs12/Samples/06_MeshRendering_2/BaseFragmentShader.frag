#version 130
precision highp float; // needed only for version 1.30

in vec3 var_LightDir;
in vec3 var_Normal;
in vec2 var_TexCoord;

uniform sampler2D diffuseTex;
uniform sampler2D glowTex;

out vec3 out_Color;
 
void main(void)
{
	float intensity = max(dot(var_Normal, var_LightDir), 0.0);
	
    vec3 diffuseColor = texture(diffuseTex, vec2(var_TexCoord.x, 1.0 - var_TexCoord.y)).rgb;
    vec3 glowColor = texture(glowTex, vec2(var_TexCoord.x, 1.0 - var_TexCoord.y)).rgb;

	vec3 color = intensity * diffuseColor + glowColor;

    out_Color = color;
}

