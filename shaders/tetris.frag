#version 330 core
out vec4 FragColor;

in vec3 f_pos;
in vec3 f_norm;
in vec3 f_col;
in vec2 f_tc;

uniform samplerCube skybox;
uniform sampler2D norm_map;

// ** Camera pos is always at origin **
void main()
{
    vec3 norm = normalize(texture(norm_map, f_tc).rgb * 2. - 1.);

    if (abs(f_norm.x) > .5) norm.x *= -sign(f_norm.x);
    if (abs(f_norm.z) > .5) norm.z *= -sign(f_norm.z);

    if (abs(f_norm.y) > .5)
    {
        float tmp = norm.y;
        norm.y = norm.x;
        norm.x = norm.y;

        norm.y *= -sign(f_norm.y);
    }

    vec3 new_pos = f_pos - norm;

    vec3 ambient = f_col * .2;

    vec3 ldir = vec3(-1., 0., 0.);
    float diff = max(dot(norm, ldir), 0.);
    vec3 diffuse = .7 * diff * f_col;

    vec3 view_dir = normalize(-new_pos);
    vec3 reflect_dir = reflect(-ldir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.), 100.f);
    vec3 specular = .6 * spec * f_col;

    vec3 result = ambient + diffuse + specular;

    float ratio = 1. / 1.4;
    vec3 I = normalize(new_pos);
    vec3 R = refract(I, mix(f_norm, norm, .2), ratio);

    FragColor = vec4(mix(result, texture(skybox, R).rgb, .5), 1.);
}
