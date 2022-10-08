CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall
INC=-Ideps/glfw/include -Ideps/glad/include -Ideps/cglm/include
LIBS=-Ldeps -lm -lglfw -lglad -lcglm

SRC=$(wildcard src/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

.PHONY: dirs clean

all: dirs target

target: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS)

obj/src/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

deps:
	deps/build.sh

dirs:
	mkdir -p obj/src

clean:
	-rm -rf obj/ a.out
