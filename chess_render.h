#ifndef CHESS_RENDER_H
#define CHESS_RENDER_H

#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_primitives.h"

typedef struct render_pack {
    ALLEGRO_DISPLAY *display;
} ChessRenderPack;

#endif