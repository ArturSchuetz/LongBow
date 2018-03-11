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
	
    vec4 diffuseColor = texture(diffuseTex, vec2(var_TexCoord.x, 1.0 - var_TexCoord.y)).rgba;

	if(diffuseColor.a == 0.0)
		discard;

	out_Color = intensity * diffuseColor.rgb;
	out_Color += 0.1 * diffuseColor.rgb;
}

