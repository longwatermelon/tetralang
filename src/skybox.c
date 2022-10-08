#include "skybox.h"
#include "shader.h"
#include <string.h>
#include <limits.h>
#include <glad/glad.h>
#include <stb/stb_image.h>

float g_verts[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

struct Skybox *skybox_alloc(const char *prefix)
{
    struct Skybox *s = malloc(sizeof(struct Skybox));

    glGenTextures(1, &s->tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s->tex);

    char str[PATH_MAX] = { 0 };
    sprintf(str, "%s", prefix);

    char *faces[6] = {
        "right.jpg", "left.jpg",
        "top.jpg", "bottom.jpg",
        "front.jpg", "back.jpg"
    };

    int width, height, nchannels;
    for (size_t i = 0; i < 6; ++i)
    {
        sprintf(&str[strlen(prefix)], "%s", faces[i]);
        str[strlen(prefix) + strlen(faces[i])] = '\0';

        unsigned char *data = stbi_load(str, &width, &height, &nchannels, 0);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            fprintf(stderr, "[skybox_alloc] Failed to load cubemap texture '%s'.\n", faces[i]);
            exit(EXIT_FAILURE);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenVertexArrays(1, &s->vao);
    glGenBuffers(1, &s->vb);

    glBindVertexArray(s->vao);
    glBindBuffer(GL_ARRAY_BUFFER, s->vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_verts), g_verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    return s;
}

void skybox_free(struct Skybox *s)
{
    glDeleteTextures(1, &s->tex);

    glDeleteVertexArrays(1, &s->vao);
    glDeleteBuffers(1, &s->vb);

    free(s);
}

void skybox_render(struct Skybox *s, RenderInfo *ri)
{
    mat4 view;
    glm_lookat((vec3){ 0.f, 0.f, 0.f }, (vec3){ 1.f, 0.f, 0.f }, (vec3){ 0.f, 1.f, 0.f }, view);

    shader_mat4(ri->shader, "view", view);
    shader_mat4(ri->shader, "projection", ri->proj);

    glBindVertexArray(s->vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s->tex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glClear(GL_DEPTH_BUFFER_BIT);
}
