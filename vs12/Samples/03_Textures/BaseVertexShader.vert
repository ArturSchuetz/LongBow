#version 130
in vec4 offset;

in vec3 in_Position;
in vec2 in_TexCoord;

out vec2 TextureCoord;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0) + offset;
	TextureCoord = in_TexCoord;
}
