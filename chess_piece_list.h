#ifndef CHESS_PIECE_LIST_H
#define CHESS_PIECE_LIST_H

#include "chess_base.h"

#define INIT_SZ 16

ChessPieceList* chess_create_new_piece_list(void);

ChessPieceList* chess_create_piece_list_section(int size);

void chess_append_piece(ChessPieceList *list, ChessPiece *piece);

ChessPiece* chess_find_piece_in_square(ChessPieceList *list, ChessSquare square);

void chess_destroy_piece_list(ChessPieceList *pieces);

#endif