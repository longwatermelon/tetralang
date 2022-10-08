#include "prog.h"
#include "util.h"

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *win = glfwCreateWindow(SCRW, SCRH, "Tetralang", 0, 0);
    glfwMakeContextCurrent(win);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, SCRW, SCRH);

    struct Prog *p = prog_alloc(win);
    prog_game(p);
    prog_free(p);

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
