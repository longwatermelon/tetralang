#ifndef RENDER_H
#define RENDER_H

#include <sys/types.h>
#include <cglm/cglm.h>

typedef enum
{
    SHADER_TETRIS,
    SHADER_SKYBOX,
    SHADER_TITLE
} ShaderType;

typedef struct
{
    unsigned int *shaders;
    size_t nshaders;

    unsigned int shader;

    mat4 proj;
} RenderInfo;

RenderInfo *ri_alloc();
void ri_free(RenderInfo *ri);

void ri_add_shader(RenderInfo *ri, ShaderType type, const char *vert, const char *frag);
void ri_use_shader(RenderInfo *ri, int i);

#endif
