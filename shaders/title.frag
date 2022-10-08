#version 330 core
out vec4 FragColor;

in vec2 f_tc;

uniform sampler2D bg;

void main()
{
    FragColor = texture(bg, f_tc);
}
