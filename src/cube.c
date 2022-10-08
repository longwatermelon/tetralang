#include "cube.h"
#include "shader.h"
#include <string.h>
#include <glad/glad.h>

static float g_verts[] = {
    // pos                // norm             // color       // texcoords
    // left
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.f, 0.f, 0.f,  0.f, 0.f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.f, 0.f, 0.f,  0.f, 1.f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.f, 0.f, 0.f,  1.f, 1.f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.f, 0.f, 0.f,  1.f, 1.f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.f, 0.f, 0.f,  1.f, 0.f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.f, 0.f, 0.f,  0.f, 0.f,

     // right
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.f, 0.f, 0.f,  1.f, 1.f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.f, 0.f, 0.f,  0.f, 1.f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.f, 0.f, 0.f,  0.f, 0.f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.f, 0.f, 0.f,  0.f, 0.f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.f, 0.f, 0.f,  1.f, 0.f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.f, 0.f, 0.f,  1.f, 1.f,

    // front
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 0.f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 0.f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 1.f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 1.f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 1.f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 0.f,

    // back
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 1.f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 0.f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 0.f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 0.f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 1.f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 1.f,

     // bottom
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 1.f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 1.f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 0.f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 0.f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 0.f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 1.f,

    // top
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 0.f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 1.f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 1.f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 1.f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.f, 0.f, 0.f,  0.f, 0.f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.f, 0.f, 0.f,  1.f, 0.f
};

struct Cube *cube_alloc(vec3 pos, vec3 col)
{
    struct Cube *c = malloc(sizeof(struct Cube));
    c->render = true;

    glm_vec3_zero(c->pos);
    glm_vec3_copy(col, c->col);

    memcpy(c->verts, g_verts, sizeof(g_verts));
    cube_move(c, pos);

    for (size_t i = 0; i < CUBE_NVERTS * CUBE_VERTLEN; i += CUBE_VERTLEN)
    {
        c->verts[i + 6] = col[0];
        c->verts[i + 7] = col[1];
        c->verts[i + 8] = col[2];
    }

    return c;
}

void cube_free(struct Cube *c)
{
    free(c);
}

void cube_move(struct Cube *c, vec3 dir)
{
    glm_vec3_add(c->pos, dir, c->pos);

    for (size_t i = 0; i < CUBE_NVERTS * CUBE_VERTLEN; i += CUBE_VERTLEN)
    {
        c->verts[i] += dir[0];
        c->verts[i + 1] += dir[1];
        c->verts[i + 2] += dir[2];
    }
}

void cube_set_col(struct Cube *c, vec3 col)
{
    glm_vec3_copy(col, c->col);

    for (size_t i = 0; i < CUBE_NVERTS * CUBE_VERTLEN; i += CUBE_VERTLEN)
    {
        c->verts[i + 6] = col[0];
        c->verts[i + 7] = col[1];
        c->verts[i + 8] = col[2];
    }
}
