#version 330 core
out vec4 FragColor;

in vec3 f_pos;
in vec3 f_norm;
in vec3 f_col;
in vec2 f_tc;

uniform samplerCube skybox;

void main()
{
    vec3 norm = normalize(f_norm);

    float ratio = 1. / 1.4;
    vec3 I = normalize(f_pos);
    vec3 R = refract(I, mix(f_norm, norm, .2), ratio);

    FragColor = vec4(mix(f_col, texture(skybox, R).rgb, .5), 1.);
}
