#!/bin/sh
cd deps
cd glfw && cmake . && make && mv src/libglfw3.a ../libglfw.a && cd ..
cd glad && gcc -c src/glad.c -Iinclude && ar rc ../libglad.a glad.o && cd ..
cd cglm && cmake . -DCGLM_STATIC=ON && make && mv libcglm.a .. && cd ..
