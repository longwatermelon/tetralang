#include "render.h"
#include "cglm/vec2.h"
#include "shader.h"
#include "util.h"
#include "texture.h"
#include <glad/glad.h>

// Image rendering
static unsigned int g_vao, g_vb;
static vec2 g_tc_max, g_tc_min;

RenderInfo *ri_alloc()
{
    RenderInfo *ri = malloc(sizeof(RenderInfo));
    ri->shaders = 0;
    ri->nshaders = 0;

    glm_perspective(glm_rad(45.f), SCRW / SCRH, .1f, 1000.f, ri->proj);

    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);

    glGenBuffers(1, &g_vb);
    glBindBuffer(GL_ARRAY_BUFFER, g_vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 3 * 2, 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    g_tc_max[0] = 1.f;
    g_tc_max[1] = 1.f;
    g_tc_min[0] = 0.f;
    g_tc_min[1] = 0.f;

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


void ri_render_image(RenderInfo *ri, struct Texture *tex, int x, int y, int w, int h, vec2 translate, vec2 scale)
{
    int lx = x;
    int ly = y;
    int ux = x + w;
    int uy = y + h;

    shader_int(ri->shader, "image", 0);
    tex_bind(tex, 0);

    shader_vec2(ri->shader, "translation", translate);
    shader_vec2(ri->shader, "scale", scale);

    float cx = SCRW / 2.f;
    float cy = SCRH / 2.f;

    float dlx = (lx - cx) / cx;
    float dly = (ly - cy) / cy;
    float dux = (ux - cx) / cx;
    float duy = (uy - cy) / cy;

    float verts[] = {
        dlx, dly, g_tc_min[0], g_tc_min[1],
        dlx, duy, g_tc_min[0], g_tc_max[1],
        dux, duy, g_tc_max[0], g_tc_max[1],

        dlx, dly, g_tc_min[0], g_tc_min[1],
        dux, dly, g_tc_max[0], g_tc_min[1],
        dux, duy, g_tc_max[0], g_tc_max[1]
    };

    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ri_set_image_tc(vec2 min, vec2 max)
{
    glm_vec2_copy(min, g_tc_min);
    glm_vec2_copy(max, g_tc_max);
}
