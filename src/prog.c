#include "prog.h"
#include "util.h"
#include <stdlib.h>

struct Prog *prog_alloc(GLFWwindow *win)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;
    p->win = win;

    return p;
}

void prog_free(struct Prog *p)
{
    free(p);
}

void prog_game(struct Prog *p)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.f, 0.f, 0.f, 1.f);

    while (p->running)
    {
        if (glfwWindowShouldClose(p->win))
            p->running = false;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCRW, SCRH);

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
