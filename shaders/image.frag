#version 330 core
out vec4 FragColor;

in vec2 f_tc;

uniform sampler2D image;

void main()
{
    FragColor = texture(image, f_tc);
}
