#version 330 core
layout (location = 0) in vec3 i_pos;

out vec3 f_pos;

uniform mat4 model;
uniform mat4 proj;

void main()
{
    f_pos = vec3(model * vec4(i_pos, 1.));
    gl_Position = proj * vec4(f_pos, 1.);
}
