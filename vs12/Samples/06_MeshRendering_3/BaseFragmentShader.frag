#version 130
precision highp float; // needed only for version 1.30

in vec3 var_lightDir;
in vec3 var_Normal;
in vec2 var_TexCoord;

uniform sampler2D diffuseTex;

out vec3 out_Color;
 
void main(void)
{
    vec4 diffuseColor = texture(diffuseTex, vec2(var_TexCoord.x, 1.0 - var_TexCoord.y)).rgba;
	
	float NdotL = max(dot(var_Normal, var_lightDir), 0.0);

	if(diffuseColor.a < 0.1)
		discard;

	out_Color = (NdotL * diffuseColor.rgb);
	out_Color += (0.1 * diffuseColor.rgb);
}