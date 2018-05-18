
CC = gcc
CFLAGS = -Wall -g $(ALLEGFLAGS)
ALLEGFLAGS = `pkg-config --cflags --libs allegro-5 allegro_image-5 allegro_primitives-5`

chess: chess.c
	@$(CC) -o $@ $^ $(CFLAGS)