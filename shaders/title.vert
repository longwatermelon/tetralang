#version 330 core
layout (location = 0) in vec2 i_pos;
layout (location = 1) in vec2 i_tc;

out vec2 f_tc;

void main()
{
    f_tc = i_tc;
    gl_Position = vec4(i_pos, 0., 1.);
}
