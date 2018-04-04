#version 130
 
in vec4 var_color;

out vec3 out_Color;
 
void main(void)
{
    out_Color = var_color.rgb;
}
