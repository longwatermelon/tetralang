#ifndef CUBE_H
#define CUBE_H

#include <cglm/cglm.h>

#define CUBE_NVERTS 36
#define CUBE_VERTLEN 11

struct Cube
{
    vec3 pos, col;
    float verts[CUBE_NVERTS * CUBE_VERTLEN];
    bool render;
};

struct Cube *cube_alloc(vec3 pos, vec3 col);
void cube_free(struct Cube *c);

void cube_move(struct Cube *c, vec3 dir);
void cube_set_col(struct Cube *c, vec3 col);

#endif
