#ifndef PROG_H
#define PROG_H

#include "render.h"
#include "board.h"
#include "skybox.h"
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Prog
{
    bool running;
    GLFWwindow *win;

    RenderInfo *ri;

    struct Board *board;
    struct Skybox *skybox;

    vec3 shake;
    float shake_begin;
};

struct Prog *prog_alloc(GLFWwindow *win);
void prog_free(struct Prog *p);

void prog_game(struct Prog *p);
void prog_title(struct Prog *p);

#endif
