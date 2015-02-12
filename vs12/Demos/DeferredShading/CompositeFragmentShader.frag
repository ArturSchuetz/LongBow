#version 330
precision highp float; // needed only for version 1.30

uniform mat4 u_ProjInv;
uniform mat4 u_View;

uniform float u_invRadius;
uniform vec3 u_lightPos;
uniform vec2 u_CanvasSize;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D depthTex;

out vec3 out_Color;
 
void main(void)
{
	vec2 texCoord = gl_FragCoord.xy / u_CanvasSize.xy;
    vec3 diffuse = vec3(texture(albedoTex, texCoord).rgb);
    vec3 normal = vec3(texture(normalTex, texCoord).rgb);
	float depth = float(texture(depthTex, texCoord).r);

	vec3 pPos = vec3( (texCoord * 2.0) - 1.0, (depth * 2.0) - 1.0 );
	vec4 hPos = vec4( pPos, 1.0 ) * u_ProjInv;
	vec3 vPos = hPos.xyz / hPos.w;

	vec3 lPos = (vec4( u_lightPos, 1.0 ) * u_View).xyz;
	vec3 lVec = (lPos - vPos) * u_invRadius;
	vec3 lightVec = normalize(lVec);

	// Attenution that falls off to zero at light radius
	const float a = 0.15;
	float atten = clamp( (1.0 + a) / (1.0 + (1.0 / a) * dot(lVec, lVec)) - a, 0.0, 1.0);

	// Lighting
	float NdotL = clamp(dot(lightVec, normalize(normal)), 0.0, 1.0);

	out_Color.xyz = atten * (diffuse * NdotL);
}
/*