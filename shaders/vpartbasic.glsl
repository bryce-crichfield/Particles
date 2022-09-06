#version 430
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_velocity;

out vec3 v_color;

void main()
{
    gl_Position = vec4(in_position, 0.0f, 1.0f);
    v_color = vec3(in_velocity, 0.0f);
    // v_color = vec3(1, 1, 0);
}
// EOF