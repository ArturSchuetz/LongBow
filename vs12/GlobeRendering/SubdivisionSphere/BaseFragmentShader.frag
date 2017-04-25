#version 130
precision highp float; // needed only for version 1.30

const float OneOverTwoPi = 1.0 / (2.0 * 3.1415926535897932384626433832795);
const float OneOverPi = 1.0 / 3.1415926535897932384626433832795;

in vec2 out_TextureCoord;
in vec3 out_Position;

uniform sampler2D diffuseTex;

out vec4 out_Color;
 
vec2 ComputeTextureCoordinates(vec3 normal)
{
    return vec2(atan(normal.y, normal.x) * OneOverTwoPi + 0.5, asin(normal.z) * OneOverPi + 0.5);
}

void main(void)
{
    vec3 normal = normalize(out_Position);

	//vec2 texCoord = out_TextureCoord; 
	vec2 texCoord = ComputeTextureCoordinates(normal);

    vec4 texel = vec4( texture(diffuseTex, texCoord).rgba);
	if(texel.a == 0.0)
		discard;
    out_Color = vec4(0.0, 0.0, 0.0, 1.0);
}
