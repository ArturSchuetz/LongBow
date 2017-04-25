#version 130
precision highp float; // needed only for version 1.30

in vec3 color;

out vec4 out_Color;

void main(void)
{
    out_Color = vec4(color.xyz, 1.0);
}
