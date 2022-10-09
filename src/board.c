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

    b->active = 0;
    b->hold = 0;
    b->can_hold = true;
    b->last_moved = 0.f;

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

    b->next_piece = rand() % 7;
    board_make_borders(b);

    b->last_cleared = 0;

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

void board_update(struct Board *b)
{
    for (size_t i = 0; i < b->npieces; ++i)
        piece_update(b->pieces[i]);

    if (!b->active)
    {
        b->active = board_spawn_piece(b);
        b->last_moved = glfwGetTime();
        b->next_piece = rand() % 8 - 1;

        if (b->next_piece < 0) b->next_piece = 0;
    }

    if (glfwGetTime() - b->last_moved > .5f)
    {
        b->last_moved = glfwGetTime();
        piece_move(b->active, (vec3){ 0.f, -1.f, 0.f });

        if (board_check_collision(b))
        {
            piece_move(b->active, (vec3){ 0.f, 1.f, 0.f });
            board_place_active(b);
            b->active = 0;

            board_clear_full_lines(b);

            b->can_hold = true;
        }
    }
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

bool board_check_collision(struct Board *b)
{
    for (size_t i = 0; i < b->active->ncubes; ++i)
    {
        struct Cube *c = b->active->cubes[i];
        int iz = c->pos[2];
        int iy = 19.f - c->pos[1];

        if (b->layout[iy * 12 + iz] == '#')
            return true;
    }

    return false;
}

void board_place_active(struct Board *b)
{
    for (size_t i = 0; i < b->active->ncubes; ++i)
    {
        struct Cube *c = b->active->cubes[i];
        int iz = c->pos[2];
        int iy = 19.f - c->pos[1];

        b->layout[iy * 12 + iz] = '#';
    }
}

bool board_move_active(struct Board *b, vec3 dir)
{
    if (!b->active)
        return false;

    piece_move(b->active, dir);

    if (board_check_collision(b))
    {
        vec3 back;
        glm_vec3_negate_to(dir, back);
        piece_move(b->active, back);

        return false;
    }

    return true;
}

void board_clear_full_lines(struct Board *b)
{
    int cleared = 0;
    int lowest = 20;

    for (size_t i = 0; i < 19; ++i)
    {
        size_t begin = i * 12;
        bool full = true;

        for (size_t z = begin + 1; z <= begin + 10; ++z)
        {
            if (b->layout[z] != '#')
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            ++cleared;
            int iy = i;

            for (size_t z = begin + 1; z <= begin + 10; ++z)
            {
                int iz = z - begin;

                for (size_t i = 0; i < b->npieces; ++i)
                {
                    for (size_t j = 0; j < b->pieces[i]->ncubes; ++j)
                    {
                        struct Cube *c = b->pieces[i]->cubes[j];
                        int ciy = 19.f - c->pos[1];
                        int ciz = c->pos[2];

                        if (ciy == iy && ciz == iz)
                            b->pieces[i]->cubes[j]->render = false;
                    }
                }

                b->layout[z] = '.';
            }

            if (iy < lowest)
                lowest = iy;
        }
    }

    if (cleared)
    {
        // Start from i = 1 to ignore border pieces
        for (size_t i = 1; i < b->npieces; ++i)
        {
            if (b->pieces[i] == b->hold)
                continue;

            for (size_t j = 0; j < b->pieces[i]->ncubes; ++j)
            {
                struct Cube *c = b->pieces[i]->cubes[j];

                if (c->pos[1] > 19.f - lowest)
                    cube_move(c, (vec3){ 0.f, -1.f * cleared, 0.f });
            }
            /* piece_move(b->pieces[i], (vec3){ 0.f, -1.f * cleared, 0.f }); */
        }

        for (int i = lowest - 1; i >= 0; --i)
        {
            for (size_t z = 1; z <= 10; ++z)
            {
                if (b->layout[i * 12 + z] == '#')
                {
                    b->layout[i * 12 + z] = '.';
                    // @ indicates newly moved piece so it doesn't get
                    // cleared by another piece moving out of that position
                    b->layout[i * 12 + 12 * cleared + z] = '@';
                }
            }
        }

        for (size_t i = 0; i < strlen(b->layout); ++i)
        {
            if (b->layout[i] == '@')
                b->layout[i] = '#';
        }
    }

    b->last_cleared = cleared;
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

struct Piece *board_spawn_piece(struct Board *b)
{
    int type = b->next_piece;
    struct Cube **cubes = malloc(sizeof(struct Cube*) * 4);

    switch (type)
    {
    case 0: // bar
    {
        vec3 pos[4] = {
            { 0.f, 19.f, 3.f },
            { 0.f, 19.f, 4.f },
            { 0.f, 19.f, 5.f },
            { 0.f, 19.f, 6.f }
        };

        vec3 col = { 75.f / 255.f, 242.f / 255.f, 231.f / 255.f };

        for (int i = 0; i < 4; ++i)
            cubes[i] = cube_alloc(pos[i], col);
    } break;
    case 1: // square
    {
        vec3 pos[4] = {
            { 0.f, 19.f, 4.f },
            { 0.f, 19.f, 5.f },
            { 0.f, 18.f, 5.f },
            { 0.f, 18.f, 4.f }
        };

        vec3 col = { 1.f, 1.f, 0.f };

        for (int i = 0; i < 4; ++i)
            cubes[i] = cube_alloc(pos[i], col);
    } break;
    case 2: // left squiggle
    {
        vec3 pos[4] = {
            { 0.f, 19.f, 4.f },
            { 0.f, 18.f, 4.f },
            { 0.f, 18.f, 5.f },
            { 0.f, 17.f, 5.f }
        };

        vec3 col = { 1.f, 100.f / 255.f, 100.f / 255.f };

        for (int i = 0; i < 4; ++i)
            cubes[i] = cube_alloc(pos[i], col);
    } break;
    case 3:
    {
        vec3 pos[4] = {
            { 0.f, 19.f, 5.f },
            { 0.f, 18.f, 5.f },
            { 0.f, 18.f, 4.f },
            { 0.f, 17.f, 4.f }
        };

        vec3 col = { 102.f / 255.f, 1.f, 102.f / 255.f };

        for (int i = 0; i < 4; ++i)
            cubes[i] = cube_alloc(pos[i], col);
    } break;
    case 4:
    {
        vec3 pos[4] = {
            { 0.f, 19.f, 4.f },
            { 0.f, 18.f, 4.f },
            { 0.f, 17.f, 4.f },
            { 0.f, 17.f, 5.f }
        };

        vec3 col = { 1.f, 178.f / 255.f, 102.f / 255.f };

        for (int i = 0; i < 4; ++i)
            cubes[i] = cube_alloc(pos[i], col);
    } break;
    case 5:
    {
        vec3 pos[4] = {
            { 0.f, 19.f, 5.f },
            { 0.f, 18.f, 5.f },
            { 0.f, 17.f, 5.f },
            { 0.f, 17.f, 4.f }
        };

        vec3 col = { 102.f / 255.f, 178.f / 255.f, 1.f };

        for (int i = 0; i < 4; ++i)
            cubes[i] = cube_alloc(pos[i], col);
    } break;
    case 6:
    {
        vec3 pos[4] = {
            { 0.f, 19.f, 5.f },
            { 0.f, 18.f, 5.f },
            { 0.f, 18.f, 4.f },
            { 0.f, 18.f, 6.f }
        };

        vec3 col = { 1.f, 153.f / 255.f, 1.f };

        for (int i = 0; i < 4; ++i)
            cubes[i] = cube_alloc(pos[i], col);
    } break;
    }

    struct Piece *p = piece_alloc(cubes, 4);
    board_add_piece(b, p);

    return p;
}

void board_swap_hold(struct Board *b)
{
    if (b->can_hold)
    {
        struct Piece *tmp = b->hold;
        b->hold = b->active;
        b->active = tmp;

        vec3 diff;

        if (b->active)
        {
            glm_vec3_sub((vec3){ 0.f, 19.f, 5.f }, b->active->cubes[0]->pos, diff);
            piece_move(b->active, diff);
        }

        if (b->hold)
        {
            glm_vec3_sub((vec3){ 0.f, 17.f, -4.f }, b->hold->cubes[1]->pos, diff);
            piece_move(b->hold, diff);
        }

        b->can_hold = false;
    }
}

void board_make_borders(struct Board *b)
{
    size_t ncubes = 10 + 20 * 2;
    struct Cube **cubes = malloc(sizeof(struct Cube*) * ncubes);
    size_t index = 0;

    for (size_t i = 0; i < 20; ++i)
    {
        b->layout[i * 12] = '#';
        cubes[index++] = cube_alloc((vec3){ 0.f, i, 0.f }, (vec3){ 1.f, 1.f, 1.f });

        b->layout[i * 12 + 11] = '#';
        cubes[index++] = cube_alloc((vec3){ 0.f, i, 11.f }, (vec3){ 1.f, 1.f, 1.f });
    }

    for (size_t i = 0; i < 10; ++i)
    {
        b->layout[((12 * 20 - 1) - 12) + (i + 2)] = '#';
        cubes[index++] = cube_alloc((vec3){ 0.f, 0.f, i + 1 }, (vec3){ 1.f, 1.f, 1.f });
    }

    /* for (size_t i = 0; i < 12 * 20; ++i) */
    /* { */
    /*     if (i % 12 == 0) putchar('\n'); */
    /*     putchar(b->layout[i]); */
    /* } */

    /* putchar('\n'); */

    struct Piece *p = piece_alloc(cubes, ncubes);
    board_add_piece(b, p);
}
