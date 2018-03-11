#version 130
precision highp float; // needed only for version 1.30

in vec3 var_LightDir;
in vec3 var_Normal;
in vec2 var_TexCoord;

uniform sampler2D diffuseTex;

out vec3 out_Color;
 
void main(void)
{
	float intensity = max(dot(var_Normal, var_LightDir), 0.0);
	
    vec3 diffuseColor = texture(diffuseTex, vec2(var_TexCoord.x, 1.0 - var_TexCoord.y)).rgb;

	out_Color = intensity * diffuseColor;
}

