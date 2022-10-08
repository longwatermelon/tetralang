#version 330 core
layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_norm;
layout (location = 2) in vec3 i_col;
layout (location = 3) in vec2 i_tc;

out vec3 f_pos;
out vec3 f_norm;
out vec3 f_col;
out vec2 f_tc;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    f_pos = vec3(model * vec4(i_pos, 1.));
    f_norm = i_norm;
    f_col = i_col;
    f_tc = i_tc;

    gl_Position = proj * view * vec4(f_pos, 1.);
    // f_pos = i_pos;
    // gl_Position = vec4(i_pos, 1.);
}
