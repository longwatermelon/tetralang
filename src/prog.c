#include "prog.h"
#include "render.h"
#include "util.h"
#include <stdlib.h>

struct Prog *prog_alloc(GLFWwindow *win)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;
    p->win = win;

    p->ri = ri_alloc();
    ri_add_shader(p->ri, SHADER_TETRIS, "shaders/tetris.vert", "shaders/tetris.frag");

    return p;
}

void prog_free(struct Prog *p)
{
    ri_free(p->ri);
    free(p);
}

void prog_game(struct Prog *p)
{
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    glClearColor(0.f, 0.f, 0.f, 1.f);

    float verts[] = {
        -1.f, -1.f, 1.f,
        -1.f, 1.f, 1.f,
        1.f, 1.f, 1.f
    };

    unsigned int vao, vb;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    while (p->running)
    {
        if (glfwWindowShouldClose(p->win))
            p->running = false;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCRW, SCRH);

        ri_use_shader(p->ri, SHADER_TETRIS);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
