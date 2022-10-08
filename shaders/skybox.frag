#version 330 core
out vec4 FragColor;

in vec3 f_tc;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, f_tc);
}
