#ifndef CHESS_ERROR_H
#define CHESS_ERROR_H

#define AL_INIT_ERROR 1
#define AL_IMAGE_INIT_ERROR 2
#define AL_PRIMITIVES_INIT_ERROR 3
#define AL_CREATE_DISPLAY_ERROR 4
#define AL_IMG_LOAD_ERROR 5
// #define AL_INIT_ERROR 6
// #define AL_INIT_ERROR 7
#define ALLOC_ERROR 6

#define PORTUGUESE 0
#define ENGLISH 1

#define CURR_LANG PORTUGUESE

void chess_error(int);

#endif