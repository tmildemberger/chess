#ifndef CHESS_MOVE_LIST_H
#define CHESS_MOVE_LIST_H

#include "chess_base.h"

#define INIT_SZ 16

ChessMoveList* chess_create_move_list(void);

void chess_destroy_move_list(ChessMoveList* moves);

#endif