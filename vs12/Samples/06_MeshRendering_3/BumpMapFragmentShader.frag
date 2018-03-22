#version 130
precision highp float; // needed only for version 1.30

in vec3 var_lightDir;
in vec2 var_TexCoord;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

out vec3 out_Color;
 
void main(void)
{
    vec4 diffuseColor = texture(diffuseTex, vec2(var_TexCoord.x, 1.0 - var_TexCoord.y)).rgba;
	vec3 textureNormal = normalize(texture(normalTex, vec2(var_TexCoord.x, 1.0 - var_TexCoord.y)).rgb * 2.0 - 1.0);
	
	float NdotL = max(dot(textureNormal, var_lightDir), 0.0);

	if(diffuseColor.a < 0.1)
		discard;

	out_Color = (NdotL * diffuseColor.rgb);
	out_Color += (0.1 * diffuseColor.rgb);
}

