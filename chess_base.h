#ifndef CHESS_BASE_H
#define CHESS_BASE_H

#define PAWN 0
#define ROOK 1
#define KNIGHT 2
#define BISHOP 3
#define QUEEN 4
#define KING 5

#define WHITE 0
#define BLACK 1
#define WAITING_PROMOTION 2
/**
 * ^^^^-> "terceiro estado" de um jogo:
 * ||||-> acontece depois de um movimento de
 * \\\\-> promoção, depois do qual se fica esperando
 *  \\\-> pela peça promovida para ser escolhida
 */

#define TABLE_WIDTH 8
#define TABLE_HEIGHT 8


#define INVALID_MOVE -1
#define UNKNOWN_MOVE 0
#define NORMAL_MOVE 1
#define CAPTURE_MOVE 2
#define CASTLING_MOVE 3
#define EN_PASSANT_MOVE 4
#define PROMOTION_MOVE 5
#define PROMOTION_PIECE 6

#include "chess_error.h"
#include <stdlib.h>

/**
 * struct piece --- ChessPiece
 * "toda peça sabe onde está,
 * o que é, de que time é
 * e quantas vezes se moveu"
 */
typedef struct piece{
    char row;
    char column;
    char name;
    char team;
    int movs;
} ChessPiece;

/**
 * a existência dessa struct é explicada quando se
 * vê como ela facilita para fazer uma lista de jogadas
 */
typedef struct move{
    char fromRow;
    char fromCol;
    char toRow;
    char toCol;
    int moveType;
} ChessMove;

typedef struct visual_piece {
    ChessPiece *piece;
    int x;
    int y;
} ChessVisualPiece;
 
/**
 * struct node --- ChessNode
 * isso serve para fazer
 * listas de peças ou jogadas
 */
typedef struct node{
    union{
        ChessPiece *piece;
        ChessMove *move;
        ChessVisualPiece *vpiece;
    };
    struct node *nxt;
    struct node *prv;
} ChessNode;

/**
 * struct game --- ChessBoard
 * essa é a estrutura principal,
 * que sabe qual é o tamanho do
 * tabuleiro, e mantém o registro
 * de quais peças estão vivas ou não
 */
typedef struct board{
    ChessNode *alive_head;
    ChessNode *not_alive_head;
    short board_width;
    short board_height;
    short movements;
    short turn;
} ChessBoard;

ChessBoard* chess_new_game(void);
ChessPiece* chess_new_piece(char col, char row, char name, char team);

ChessPiece* chess_piece_in_pos(ChessBoard *play, char col, char row);
ChessMove* chess_create_move(ChessBoard *play, ChessPiece *piece,
                                                 char toCol, char toRow);
// ChessMove* chess_is_valid_move(ChessBoard *play, ChessPiece *piece,
//                                                  char toCol, char toRow);
int chess_piece_cant_move(ChessBoard *play, ChessPiece *piece);
int chess_analize_move(ChessBoard *play, ChessPiece *piece,
                                    char toCol, char toRow);
void chess_apply_move(ChessBoard *play, ChessMove *move);
int chess_squares_satifies(ChessBoard *play, char fromCol, char fromRow,
                            char toCol, char toRow, 
                            int (*func)(ChessBoard*, char, char));
int chess_empty_square(ChessBoard* play, char col, char row);
int chess_safe_square(ChessBoard* play, char col, char row);

void chess_game_over(ChessBoard *play);
void chess_destroy_piece(ChessPiece *pic);
void chess_destroy_move(ChessMove *move);

#endif