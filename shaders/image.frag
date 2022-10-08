#version 330 core
out vec4 FragColor;

in vec2 f_tc;

uniform sampler2D image;

void main()
{
    vec4 col = texture(image, f_tc);
    FragColor = col;
}
