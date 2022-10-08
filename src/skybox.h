#ifndef SKYBOX_H
#define SKYBOX_H

#include "render.h"

struct Skybox
{
    unsigned int tex;
    unsigned int vao, vb;
};

struct Skybox *skybox_alloc(const char *prefix);
void skybox_free(struct Skybox *s);

void skybox_render(struct Skybox *s, RenderInfo *ri);

#endif
