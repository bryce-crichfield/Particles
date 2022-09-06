#version 430

precision highp float;

in vec3 v_color;

out vec4 f_color;

void main()
{
    f_color = vec4(v_color, 1.0);
}
// EOF