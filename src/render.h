#ifndef RENDER_H
#define RENDER_H

#include <sys/types.h>
#include <cglm/cglm.h>

struct Texture;

typedef enum
{
    SHADER_TETRIS,
    SHADER_SKYBOX,
    SHADER_IMAGE
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

// Call shader for image beforehand
void ri_render_image(RenderInfo *ri, struct Texture *tex, int x, int y, int w, int h, vec2 translate, vec2 scale);
void ri_set_image_tc(vec2 min, vec2 max);

#endif
