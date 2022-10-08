#ifndef PIECE_H
#define PIECE_H

#include "render.h"
#include "cube.h"

struct Piece
{
    struct Cube **cubes;
    size_t ncubes;

    float *verts;
    size_t nverts;

    vec3 col;
};

// Piece takes ownership of cubes list
struct Piece *piece_alloc(struct Cube **cubes, size_t ncubes);
void piece_free(struct Piece *p);

void piece_update(struct Piece *p);

void piece_move(struct Piece *p, vec3 dir);
void piece_rotate(struct Piece *p);

void piece_set_col(struct Piece *p, vec3 col);

float *piece_verts(struct Piece *p, size_t *len);

#endif
