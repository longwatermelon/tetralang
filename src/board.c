#include "board.h"
#include "shader.h"
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Board *board_alloc()
{
    struct Board *b = malloc(sizeof(struct Board));
    b->layout = calloc(12 * 20 + 1, sizeof(char));

    for (size_t i = 0; i < 20; ++i)
        strcat(b->layout, "............");

    b->pieces = 0;
    b->npieces = 0;

    b->verts = 0;
    b->nverts = 0;

    glGenVertexArrays(1, &b->vao);
    glBindVertexArray(b->vao);

    glGenBuffers(1, &b->vb);
    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

    // verts
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, CUBE_VERTLEN * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // norms
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, CUBE_VERTLEN * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // colors
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, CUBE_VERTLEN * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    // tc
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, CUBE_VERTLEN * sizeof(float), (void*)(sizeof(float) * 9));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return b;
}

void board_free(struct Board *b)
{
    for (size_t i = 0; i < b->npieces; ++i)
        piece_free(b->pieces[i]);

    free(b->pieces);

    glDeleteVertexArrays(1, &b->vao);
    glDeleteBuffers(1, &b->vb);

    free(b->layout);
    free(b);
}

void board_render(struct Board *b, RenderInfo *ri)
{
    size_t len = board_fill_verts(b);

    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * b->nverts, b->verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){ 30.f, -10.f, -4.5f });
    shader_mat4(ri->shader, "model", model);

    glBindVertexArray(b->vao);
    glDrawArrays(GL_TRIANGLES, 0, len / CUBE_VERTLEN);
    glBindVertexArray(0);
}

size_t board_fill_verts(struct Board *b)
{
    size_t index = 0;

    for (size_t i = 0; i < b->npieces; ++i)
    {
        size_t len;
        float *pverts = piece_verts(b->pieces[i], &len);

        memcpy(b->verts + index, pverts, sizeof(float) * len);
        index += len;
    }

    return index;
}

void board_add_piece(struct Board *b, struct Piece *p)
{
    b->pieces = realloc(b->pieces, sizeof(struct Piece*) * ++b->npieces);
    b->pieces[b->npieces - 1] = p;

    b->nverts += p->nverts;
    b->verts = realloc(b->verts, sizeof(float) * b->nverts);

    glBindBuffer(GL_ARRAY_BUFFER, b->vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * b->nverts, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
