#ifndef PROG_H
#define PROG_H

#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Prog
{
    bool running;
    GLFWwindow *win;
};

struct Prog *prog_alloc(GLFWwindow *win);
void prog_free(struct Prog *p);

void prog_game(struct Prog *p);

#endif
