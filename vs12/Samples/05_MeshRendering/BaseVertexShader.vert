#version 140

in vec3 in_Position;
in vec2 in_TexCoord;

out vec2 TextureCoord;

uniform mat4 u_ModelViewProj;

void main(void)
{
    gl_Position = u_ModelViewProj * vec4(in_Position, 1.0);
	TextureCoord = in_TexCoord;
}