#version 330

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;

out vec2 TextureCoord;
out vec3 Normal;

uniform mat4 u_ViewProj;
uniform mat4 u_View;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0) * u_ViewProj; // multiplying a vector from the left to a matrix corresponds to multiplying it from the right to the transposed matrix
	TextureCoord = in_TexCoord;
	Normal.xyz = (vec4(in_Normal, 0.0) * u_View).xyz;
}
/*