#version 330 core
out vec4 FragColor;

in vec3 f_pos;
in vec3 f_norm;
in vec3 f_col;
in vec2 f_tc;

void main()
{
    FragColor = vec4(f_col, 1.);
}
