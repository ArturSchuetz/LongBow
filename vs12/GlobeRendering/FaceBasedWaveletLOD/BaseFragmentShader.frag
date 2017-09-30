#version 140
precision highp float; // needed only for version 1.30

const float OneOverTwoPi = 1.0 / (2.0 * 3.1415926535897932384626433832795);
const float OneOverPi = 1.0 / 3.1415926535897932384626433832795;

in vec3 out_Position;
in vec3 out_Normal;
in float out_Color;

uniform vec4 u_LightDirection;
uniform float u_renderMode;
uniform sampler2D diffuseTex;

out vec4 Color;

vec2 ComputeTextureCoordinates(vec3 normal)
{
    return vec2(atan(normal.y, normal.x) * OneOverTwoPi + 0.5, asin(normal.z) * OneOverPi + 0.5);
}

void main(void)
{
	vec3 normal = normalize(out_Position);
	vec2 texCoord = ComputeTextureCoordinates(normal);
	vec4 texel = vec4(texture(diffuseTex, texCoord).rgba);

    vec3 lightDir = normalize(u_LightDirection.xyz);
	vec3 n = normalize(out_Normal);
    float NdotL = max(dot(n, lightDir), 0.0);
    Color = u_renderMode * (texel * NdotL);

    Color += (1.0 - u_renderMode) * (vec4(out_Color, out_Color, out_Color, 1.0));
}






























