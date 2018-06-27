#ifndef CHESS_ERROR_H
#define CHESS_ERROR_H

// talvez algum dia haja uma diferença entre erros fatais e 
// outros erros menos fatais

typedef enum error_types{
    AL_INIT_ERROR = 1,
    AL_IMAGE_INIT_ERROR,
    AL_PRIMITIVES_INIT_ERROR,
    AL_FONT_INIT_ERROR,
    AL_TTF_INIT_ERROR,
    AL_INSTALL_MOUSE_ERROR,
    AL_INSTALL_KEYBOARD_ERROR,
    AL_LOAD_FONT_ERROR,
    AL_CREATE_TIMER_ERROR,
    AL_CREATE_DISPLAY_ERROR,
    AL_IMG_LOAD_ERROR,
    AL_CREATE_BITMAP_ERROR,
    AL_CREATE_EVENT_QU_ERROR,
    ALLOC_ERROR
} ChessErrors;

#define PORTUGUESE 0
#define ENGLISH 1

#define CURR_LANG PORTUGUESE

void chess_error(int);

#endif