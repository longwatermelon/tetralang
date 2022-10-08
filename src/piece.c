#include "piece.h"
#include "shader.h"
#include <string.h>
#include <glad/glad.h>

struct Piece *piece_alloc(struct Cube **cubes, size_t ncubes)
{
    struct Piece *p = malloc(sizeof(struct Piece));

    p->ncubes = ncubes;
    p->cubes = cubes;

    p->nverts = p->ncubes * CUBE_VERTLEN * CUBE_NVERTS;
    p->verts = malloc(sizeof(float) * p->nverts);

    glm_vec3_copy(p->cubes[0]->col, p->col);

    return p;
}

void piece_free(struct Piece *p)
{
    for (size_t i = 0; i < p->ncubes; ++i)
        cube_free(p->cubes[i]);

    free(p->cubes);
    free(p);
}

void piece_update(struct Piece *p)
{
    vec3 diff;
    glm_vec3_sub(p->col, p->cubes[0]->col, diff);
    glm_vec3_scale(diff, 1.f / 5.f, diff);
    glm_vec3_add(p->cubes[0]->col, diff, diff);

    piece_set_col(p, diff);
}

void piece_move(struct Piece *p, vec3 dir)
{
    for (size_t i = 0; i < p->ncubes; ++i)
        cube_move(p->cubes[i], dir);
}

void piece_rotate(struct Piece *p)
{
    vec3 center;
    glm_vec3_copy(p->cubes[1]->pos, center);

    for (size_t i = 0; i < p->ncubes; ++i)
    {
        int dz = p->cubes[i]->pos[2] - center[2];
        int dy = p->cubes[i]->pos[1] - center[1];

        dz = -dz;
        dy = -dy;

        cube_move(p->cubes[i], (vec3){
            0.f,
            (dz + center[1]) - p->cubes[i]->pos[1],
            (-dy + center[2]) - p->cubes[i]->pos[2],
        });
    }
}

void piece_set_col(struct Piece *p, vec3 col)
{
    for (size_t i = 0; i < p->ncubes; ++i)
        cube_set_col(p->cubes[i], col);
}

float *piece_verts(struct Piece *p, size_t *len)
{
    size_t index = 0;

    for (size_t i = 0; i < p->ncubes; ++i)
    {
        if (!p->cubes[i]->render)
            continue;

        memcpy(p->verts + index, p->cubes[i]->verts, sizeof(p->cubes[i]->verts));
        index += CUBE_VERTLEN * CUBE_NVERTS;
    }

    *len = index;
    return p->verts;
}
