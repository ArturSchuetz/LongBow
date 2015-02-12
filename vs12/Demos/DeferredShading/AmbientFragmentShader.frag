#version 330
precision highp float; // needed only for version 1.30

in vec2 TextureCoord;

uniform mat4 u_ProjInv;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D depthTex;

out vec3 out_Color;
 
void main(void)
{
    vec3 diffuse = vec3(texture(albedoTex, TextureCoord).rgb);
    vec3 normal = vec3(texture(normalTex, TextureCoord).rgb);
	float depth = float(texture(depthTex, TextureCoord).r);

	vec3 pPos = vec3( (TextureCoord * 2.0) - 1.0, (depth * 2.0) - 1.0 );
	vec4 hPos = vec4( pPos, 1.0 ) * u_ProjInv;
	vec3 vPos = hPos.xyz / hPos.w;
	float dist = length(vPos);

	// Lighting
	float NdotL = clamp(dot(normalize(-vPos), normalize(normal)), 0.0, 1.0);
	out_Color.xyz = (1.0 / dist) * (diffuse * NdotL);
}
/*