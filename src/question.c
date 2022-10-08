#include "question.h"
#include <stdlib.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <stb/stb_image.h>

struct Question *question_alloc(const char *path, int ans)
{
    struct Question *q = malloc(sizeof(struct Question));
    q->answer = ans;

    stbi_set_flip_vertically_on_load(true);
    q->tex = tex_alloc(path);
    stbi_set_flip_vertically_on_load(false);

    return q;
}

void question_free(struct Question *q)
{
    tex_free(q->tex);
    free(q);
}
