src = $(wildcard *.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.dep)

CC = gcc
CPP = cpp
CFLAGS = -Wall -g $(ALLEGFLAGS)
ALLEGFLAGS = `pkg-config --cflags --libs allegro-5 allegro_image-5 allegro_primitives-5`

-include $(deps) 

%.dep: %.c
	$(CPP) $(CFLAGS) %< -MM -MT $(@:.dep=.o) > $@

chess: $(obj)
	@$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(dep)