#ifndef QUESTION_H
#define QUESTION_H

#include "texture.h"

struct Question
{
    struct Texture *tex;
    int answer;
};

struct Question *question_alloc(const char *path, int ans);
void question_free(struct Question *q);

#endif
