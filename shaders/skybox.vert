#version 330 core
layout (location = 0) in vec3 i_pos;

out vec3 f_tc;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * vec4(i_pos, 1.);
    f_tc = i_pos;
}
