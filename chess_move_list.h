#ifndef CHESS_MOVE_LIST_H
#define CHESS_MOVE_LIST_H

#include "chess_base.h"

#define INIT_SZ 16

ChessMoveList* chess_create_move_list(void);

ChessMoveList* chess_create_move_list_section(int size);

void chess_append_move(ChessMoveList *list, ChessMove move);

ChessMove chess_last_move(ChessMoveList *list);

void chess_destroy_move_list(ChessMoveList* moves);

#endif