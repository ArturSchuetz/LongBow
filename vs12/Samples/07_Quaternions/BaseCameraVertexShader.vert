#version 140

in vec3 in_Position;
in vec2 in_TexCoord;

out vec2 TextureCoord;

uniform mat4 u_ModelViewProj;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0) * u_ModelViewProj; // multiplying a vector from the left to a matrix corresponds to multiplying it from the right to the transposed matrix
	TextureCoord = in_TexCoord;
}