#include "prog.h"
#include "cube.h"
#include "piece.h"
#include "question.h"
#include "render.h"
#include "shader.h"
#include "board.h"
#include "skybox.h"
#include "stb/stb_image.h"
#include "util.h"
#include "texture.h"
#include <stdlib.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

struct Prog *g_prog;

#define QWIDTH 400
#define QHEIGHT 240

static void key_callback(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_LEFT)
            board_move_active(g_prog->board, (vec3){ 0.f, 0.f, -1.f });

        if (key == GLFW_KEY_RIGHT)
            board_move_active(g_prog->board, (vec3){ 0.f, 0.f, 1.f });

        if (key == GLFW_KEY_DOWN)
            board_move_active(g_prog->board, (vec3){ 0.f, -1.f, 0.f });

        if (key == GLFW_KEY_UP && g_prog->board->active)
            piece_rotate(g_prog->board->active);

        if (key == GLFW_KEY_SPACE)
        {
            while (board_move_active(g_prog->board, (vec3){ 0.f, -1.f, 0.f }))
                ;

            g_prog->board->last_moved = glfwGetTime() - .5f;
            g_prog->shake_begin = glfwGetTime();
        }

        if (key == GLFW_KEY_C)
        {
            board_swap_hold(g_prog->board);
        }
    }
}

struct Prog *prog_alloc(GLFWwindow *win)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;
    p->win = win;

    p->ri = ri_alloc();
    ri_add_shader(p->ri, SHADER_TETRIS, "shaders/tetris.vert", "shaders/tetris.frag");
    ri_add_shader(p->ri, SHADER_SKYBOX, "shaders/skybox.vert", "shaders/skybox.frag");
    ri_add_shader(p->ri, SHADER_IMAGE, "shaders/image.vert", "shaders/image.frag");

    p->skybox = skybox_alloc("res/skybox/");

    glm_vec3_zero(p->shake);
    p->shake_begin = -100.f;

    p->board = 0;

    p->questions = 0;
    p->nquestions = 0;

    prog_load_questions(p);

    p->curr_q = rand() % p->nquestions;

    p->correct = 0;
    p->total = 0;

    g_prog = p;
    return p;
}

void prog_free(struct Prog *p)
{
    for (size_t i = 0; i < p->nquestions; ++i)
        question_free(p->questions[i]);
    free(p->questions);

    ri_free(p->ri);
    skybox_free(p->skybox);
    free(p);
}

void prog_game(struct Prog *p)
{
    stbi_set_flip_vertically_on_load(true);
    struct Texture *checkmark = tex_alloc("res/check.png");
    struct Texture *cross = tex_alloc("res/cross.png");

    struct Texture *ranking_text = tex_alloc("res/ranking-text.png");
    struct Texture *scores = tex_alloc("res/scores.png");

    struct Texture *logo = tex_alloc("res/tetralang-title.png");
    stbi_set_flip_vertically_on_load(false);
    glfwSetKeyCallback(p->win, key_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    mat4 view;
    glm_look((vec3){ 0.f, 0.f, 0.f }, (vec3){ 1.f, 0.f, 0.f }, (vec3){ 0.f, 1.f, 0.f }, view);

    p->board = board_alloc();

    struct Texture *norm_map = tex_alloc("res/normal.jpg");

    glClearColor(0.f, 0.f, 0.f, 1.f);

    float last_submit = glfwGetTime();
    float check_expand = 1.f;
    float check_expand_vel = .1f;
    float check_expand_acc = -.01f;
    bool submit_correct = false;
    bool show_checkmark = false;

    float question_view_time = 3.f;
    // vec4 qdefault_viewport = { SCRW - QWIDTH, SCRH - QHEIGHT, QWIDTH, QHEIGHT };
    vec4 qdefault_viewport = { 0.f, 0.f, QWIDTH, QHEIGHT };
    vec4 center_viewport = { SCRW / 2.f - QWIDTH, SCRH / 2.f - QHEIGHT, QWIDTH * 2.f, QHEIGHT * 2.f };
    vec4 question_viewport;
    glm_vec4_copy(qdefault_viewport, question_viewport);

    while (p->running)
    {
        if (glfwWindowShouldClose(p->win))
            p->running = false;

        if (glfwGetTime() - last_submit > question_view_time)
            board_update(p->board);

        if (p->board->last_cleared)
        {
            if (p->board->last_cleared == p->questions[p->curr_q]->answer)
            {
                submit_correct = true;
                last_submit = glfwGetTime();
                ++p->correct;
            }
            else
            {
                submit_correct = false;
                last_submit = glfwGetTime();
            }

            show_checkmark = true;

            ++p->total;

            size_t prev = p->curr_q;
            while (p->curr_q == prev)
                p->curr_q = rand() % p->nquestions;

            p->board->last_cleared = 0;
        }

        if (glfwGetTime() - p->shake_begin < .05f)
        {
            p->shake[1] = (float)(rand() % 100 - 50) / 300.f;
            p->shake[2] = (float)(rand() % 100 - 50) / 300.f;
            glm_look(p->shake, (vec3){ 1.f, 0.f, 0.f }, (vec3){ 0.f, 1.f, 0.f }, view);
        }
        else
        {
            glm_look((vec3){ 0.f, 0.f, 0.f }, (vec3){ 1.f, 0.f, 0.f }, (vec3){ 0.f, 1.f, 0.f }, view);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Game
        glViewport(0, 0, SCRW, SCRH);

        ri_use_shader(p->ri, SHADER_SKYBOX);
        skybox_render(p->skybox, p->ri);

        ri_use_shader(p->ri, SHADER_TETRIS);
        shader_mat4(p->ri->shader, "proj", p->ri->proj);
        shader_mat4(p->ri->shader, "view", view);

        mat4 model;
        glm_mat4_identity(model);
        shader_mat4(p->ri->shader, "model", model);

        shader_int(p->ri->shader, "skybox", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, p->skybox->tex);

        shader_int(p->ri->shader, "norm_map", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, norm_map->id);

        board_render(p->board, p->ri);

        // Game checkmark
        if (show_checkmark)
        {
            check_expand += check_expand_vel;
            check_expand_vel += check_expand_acc;
            if (check_expand < 0.f)
            {
                show_checkmark = false;
                check_expand = 1.f;
                check_expand_vel = .1f;
            }
            else
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glViewport(0, 0, SCRW, SCRH);
                ri_use_shader(p->ri, SHADER_IMAGE);
                glDisable(GL_CULL_FACE);
                ri_render_image(p->ri, submit_correct ? checkmark : cross, SCRW / 2.f - 50.f, SCRH / 2.f - 50.f, 100.f, 100.f, (vec2){ 0.f, 0.f }, (vec2){ check_expand, check_expand });
                glEnable(GL_CULL_FACE);

                glDisable(GL_BLEND);
            }
        }

        // Questions
        ri_use_shader(p->ri, SHADER_IMAGE);
        glViewport(question_viewport[0], question_viewport[1], question_viewport[2], question_viewport[3]);

        if (glfwGetTime() - last_submit < question_view_time && glfwGetTime() - last_submit > .4f)
        {
            question_viewport[0] += (center_viewport[0] - question_viewport[0]) / 8.f;
            question_viewport[1] += (center_viewport[1] - question_viewport[1]) / 8.f;
            question_viewport[2] += (center_viewport[2] - question_viewport[2]) / 8.f;
            question_viewport[3] += (center_viewport[3] - question_viewport[3]) / 8.f;
        }
        else
        {
            question_viewport[0] += (qdefault_viewport[0] - question_viewport[0]) / 8.f;
            question_viewport[1] += (qdefault_viewport[1] - question_viewport[1]) / 8.f;
            question_viewport[2] += (qdefault_viewport[2] - question_viewport[2]) / 8.f;
            question_viewport[3] += (qdefault_viewport[3] - question_viewport[3]) / 8.f;
        }

        glDisable(GL_CULL_FACE);
        // if (glfwGetTime() - last_submit > question_view_time)
        //     ri_render_image(p->ri, p->questions[p->curr_q]->tex, 0, SCRH - QHEIGHT, SCRW, QHEIGHT, (vec2){ 0.f, 0.f }, (vec2){ 1.f, 1.f });
        ri_render_image(p->ri, p->questions[p->curr_q]->tex, 0, 0, SCRW, SCRH, (vec2){ 0.f, 0.f }, (vec2){ 1.f, 1.f });

        // Logo
        {
            glViewport(SCRW - QWIDTH, 0, QWIDTH, SCRH);
            glEnable(GL_BLEND);
            // 1280x200
            ri_render_image(p->ri, logo, (SCRW - QWIDTH) / 2.f - 130.f, SCRH - 100.f, 240.f * SCRW / QWIDTH, 240.f / 1280.f * 200.f, (vec2){ 0.f, 0.f }, (vec2){ 1.f, 1.f });
            glDisable(GL_BLEND);
        }

        // Ranking
        {
            glViewport(SCRW - QWIDTH, 0, QWIDTH, SCRH);
            float ranking_text_w = 75.f * (SCRW / QWIDTH);
            float resize = 1.3f;
            float start_y = SCRH - 150.f;

            glEnable(GL_BLEND);

            ri_render_image(p->ri, ranking_text, (SCRW - QWIDTH) / 2.f, start_y, ranking_text_w * resize, 18.f * resize, (vec2){ 0.f, 0.f }, (vec2){ 1.f, 1.f });

            float tc_x = .2f;

            if (p->total != 0)
            {
                float percentage = (float)p->correct / p->total;

                if (percentage >= .9f) tc_x = .2f;
                else if (percentage >= .8f) tc_x = .4f;
                else if (percentage >= .7f) tc_x = .6f;
                else if (percentage >= .6f) tc_x = .8f;
                else tc_x = 1.f;
            }
            ri_set_image_tc((vec2){ tc_x - .2f, 0.f }, (vec2){ tc_x, 1.f });
            ri_render_image(p->ri, scores, (SCRW - QWIDTH) / 2.f + ranking_text_w * resize + 10.f, start_y + 2.f, 18.f * (SCRW / QWIDTH) * resize, 18.f * resize, (vec2){ 0.f, 0.f }, (vec2){ 1.f, 1.f });
            ri_set_image_tc((vec2){ 0.f, 0.f }, (vec2){ 1.f, 1.f });

            glDisable(GL_BLEND);
        }

        glEnable(GL_CULL_FACE);

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }

    tex_free(logo);
    tex_free(scores);
    tex_free(ranking_text);
    tex_free(cross);
    tex_free(checkmark);
    tex_free(norm_map);
    board_free(p->board);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void prog_title(struct Prog *p)
{
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    struct Texture *bg = tex_alloc("res/title.jpg");
    struct Texture *btn = tex_alloc("res/start_button.png");
    struct Texture *btn_hover = tex_alloc("res/start_button_hover.png");
    struct Texture *title_art = tex_alloc("res/tetralang-title.png");

    float expand = 0.f;
    bool pressed = false;

    bool running = true;

    while (running)
    {
        if (glfwWindowShouldClose(p->win))
        {
            running = false;
            p->running = false;
        }

        double mx, my;
        glfwGetCursorPos(p->win, &mx, &my);

        bool hover = false;

        if (mx >= SCRW / 2.f - 100.f && mx <= SCRW / 2.f + 100.f &&
            my >= SCRH / 2.f - 50.f && my <= SCRH / 2.f + 50.f)
        {
            if (glfwGetMouseButton(p->win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                expand += (-.1f - expand) / 3.f;
                pressed = true;
            }
            else
            {
                expand += (.15f - expand) / 5.f;
            }

            hover = true;
        }
        else
        {
            expand += -expand / 5.f;
        }

        if (glfwGetMouseButton(p->win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && pressed)
            running = false;

        glClear(GL_COLOR_BUFFER_BIT);

        ri_use_shader(p->ri, SHADER_IMAGE);
        ri_render_image(p->ri, bg, 0, 0, SCRW, SCRH, (vec2){ 0.f, 0.f }, (vec2){ 1.f, 1.f });
        ri_render_image(p->ri, hover ? btn_hover : btn, SCRW / 2.f - 100.f, SCRH / 2.f - 50.f, 200, 100, (vec2){ 0.f, 0.f }, (vec2){ 1.f + expand, 1.f + expand });
        // 1280x720
        ri_render_image(p->ri, title_art, SCRW / 2.f - 300.f, SCRH - 150.f, 600.f, 600.f / 1280.f * 200.f, (vec2){ 0.f, 0.f }, (vec2){ 1.f, 1.f });

        glfwSwapBuffers(p->win);
        glfwPollEvents();
    }

    tex_free(title_art);
    tex_free(btn_hover);
    tex_free(btn);
    tex_free(bg);

    glDisable(GL_BLEND);
}

#define QUESTION(s, a) question_alloc("res/questions/" s, a)

void prog_load_questions(struct Prog *p)
{
    struct Question *questions[] = {
        QUESTION("wo.png", 1),
        QUESTION("m_ta.png", 3),
        QUESTION("f_ta.png", 1),
        QUESTION("ni.png", 2),
        QUESTION("shi.png", 2),
        QUESTION("bu.png", 4),
        QUESTION("da.png", 1),
        QUESTION("ren.png", 2),
        QUESTION("xiao.png", 2),
        QUESTION("liang.png", 3)
    };

    p->nquestions = sizeof(questions) / sizeof(struct Question*);
    p->questions = malloc(sizeof(struct Question*) * p->nquestions);

    for (size_t i = 0; i < p->nquestions; ++i)
        p->questions[i] = questions[i];
}
