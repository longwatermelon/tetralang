#version 330 core
layout (location = 0) in vec2 i_pos;
layout (location = 1) in vec2 i_tc;

out vec2 f_tc;

uniform vec2 scale;
uniform vec2 translation;

void main()
{
    f_tc = i_tc;
    gl_Position = vec4(vec2(i_pos.x * scale.x, i_pos.y * scale.y) + translation, 0., 1.);
}
