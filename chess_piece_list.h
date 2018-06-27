#ifndef CHESS_PIECE_LIST_H
#define CHESS_PIECE_LIST_H

#include "chess_base.h"

#define INIT_SZ 16

ChessPieceList* chess_create_new_piece_list(void);

ChessPieceList* chess_create_piece_list_section(int size);

void chess_append_piece(ChessPieceList *list, ChessPiece *piece);

ChessPiece* chess_find_piece_in_square(ChessPieceList *list, ChessSquare square);

ChessPiece* chess_find_not_alive_piece(ChessPieceList *list, 
                                       ChessSquare square, 
                                       PiecesType pType,
                                       unsigned char team);

ChessPiece* chess_find_king(ChessPieceList *list, unsigned char team);

ChessPiece* chess_piece_index(ChessPieceList *list, unsigned int idx);

void chess_destroy_last_piece(ChessPieceList *pieces);

void chess_destroy_piece_list(ChessPieceList *pieces);

#endif