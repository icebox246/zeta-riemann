FLAGS = `pkg-config sdl2 --cflags --libs` -lm

zeta: main.c
	gcc $^ -o $@ ${FLAGS}


