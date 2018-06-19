#ifndef CHESS_VPIECE_LIST_H
#define CHESS_VPIECE_LIST_H

#include "chess_base.h"

#define INIT_SZ 16

ChessVPieceList* chess_create_new_vpiece_list(void);

ChessVPieceList* chess_create_vpiece_list_section(int size);

void chess_append_vpiece(ChessVPieceList *list, ChessVisualPiece *vpiece);

ChessVisualPiece* chess_find_vpiece_with_piece(ChessVPieceList *list, ChessPiece *piece);

ChessVisualPiece* chess_v_piece_index(ChessVPieceList *list, unsigned int idx);

void chess_destroy_vpiece_list(ChessVPieceList *vpieces);

#endif