#include "render.h"
#include "shader.h"
#include "util.h"
#include <glad/glad.h>

RenderInfo *ri_alloc()
{
    RenderInfo *ri = malloc(sizeof(RenderInfo));
    ri->shaders = 0;
    ri->nshaders = 0;

    glm_perspective(glm_rad(45.f), SCRW / SCRH, .1f, 1000.f, ri->proj);

    return ri;
}

void ri_free(RenderInfo *ri)
{
    for (size_t i = 0; i < ri->nshaders; ++i)
        glDeleteShader(ri->shaders[i]);

    free(ri->shaders);
    free(ri);
}

void ri_add_shader(RenderInfo *ri, ShaderType type, const char *vert, const char *frag)
{
    if (ri->nshaders != type)
    {
        fprintf(stderr, "[ri_add_shader] Error: ShaderType %d is not associated with vertex shader "
                "'%s' and fragment shader '%s'.\n", type, vert, frag);
        exit(EXIT_FAILURE);
    }

    ri->shaders = realloc(ri->shaders, sizeof(unsigned int) * ++ri->nshaders);
    ri->shaders[ri->nshaders - 1] = shader_create(vert, frag);
}

void ri_use_shader(RenderInfo *ri, int i)
{
    ri->shader = ri->shaders[i];
    glUseProgram(ri->shader);
}
