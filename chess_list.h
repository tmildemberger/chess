#ifndef CHESS_LIST_H
#define CHESS_LIST_H

#include "chess_base.h"

ChessNode* chess_list_create(void);

void chess_list_append_new_piece(ChessNode *head, ChessPiece *piece);
void chess_list_append_new_v_piece(ChessNode *head, ChessVisualPiece *vpiece);
void chess_list_append_new_move(ChessNode *head, ChessMove *move);
void chess_list_append_node(ChessNode *head, ChessNode *node);
void chess_list_insert_before(ChessNode *at_right, ChessNode *to_be_inserted);

void chess_list_destroy_empty(ChessNode *head);

ChessNode* chess_list_find_piece(ChessNode *head, ChessPiece *piece);
ChessVisualPiece* chess_list_find_v_piece_with_piece(ChessNode *head, ChessPiece *piece);
ChessNode* chess_list_find_v_piece(ChessNode *head, ChessVisualPiece *vpiece);
void chess_list_remove_node(ChessNode *node);

#endif