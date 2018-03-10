#version 130
precision highp float; // needed only for version 1.30

in vec3 var_Color;

out vec3 out_Color;
 
void main(void)
{
    out_Color = var_Color;
}