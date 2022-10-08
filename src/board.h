#ifndef BOARD_H
#define BOARD_H

#include "render.h"
#include "piece.h"

struct Board
{
    char *layout;

    struct Piece **pieces;
    size_t npieces;

    float *verts;
    size_t nverts;

    unsigned int vao, vb;
};

struct Board *board_alloc();
void board_free(struct Board *b);

void board_render(struct Board *b, RenderInfo *ri);
size_t board_fill_verts(struct Board *b);

void board_add_piece(struct Board *b, struct Piece *p);

void board_make_borders(struct Board *b);

#endif
