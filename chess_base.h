#ifndef CHESS_BASE_H
#define CHESS_BASE_H

typedef enum pieces_type {  PAWN = 0, 
                            ROOK, 
                            KNIGHT, 
                            BISHOP, 
                            QUEEN, 
                            KING
} PiecesType;

#define WHITE 0
#define BLACK 1

#define TABLE_WIDTH 8
#define TABLE_HEIGHT 8

typedef enum move_type {
    INVALID_MOVE = 0,
    UNKNOWN_MOVE,
    NORMAL_MOVE,
    CAPTURE_MOVE,
    CASTLING_MOVE,
    EN_PASSANT_MOVE,
    PROMOTION_TO_ROOK,
    PROMOTION_TO_KNIGHT,
    PROMOTION_TO_BISHOP,
    PROMOTION_TO_QUEEN
} MoveType;

#include "chess_error.h"
#include <stdlib.h>
#include <stdint.h>

typedef struct square{
    unsigned char col;
    unsigned char row;
} ChessSquare;

/**
 * struct piece --- ChessPiece
 * "toda peça sabe onde está,
 * o que é, de que time é
 * e quantas vezes se moveu"
 */
typedef struct piece{
    ChessSquare pos;
    char heading;
    unsigned char team : 7;
    unsigned char alive : 1;
    unsigned int movs;
    PiecesType type;
} ChessPiece;

/**
 * a existência dessa struct é explicada quando se
 * vê como ela facilita para fazer uma lista de jogadas
 */
typedef struct move{
    ChessSquare from;
    ChessSquare to;
    MoveType type;
} ChessMove;

typedef struct visual_piece {
    ChessPiece *piece;
    unsigned int x;
    unsigned int y;
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
    ChessNode *apieces;
    short board_width;
    short board_height;
    short movements;
    short turn;
} ChessBoard;

typedef struct move_list{
    struct move_list *nxt;
    unsigned int used_size;
    unsigned int allocated_size;
    ChessMove moves[]; 
} ChessMoveList;

typedef struct match{
    ChessBoard board;
    ChessMoveList *record;
} ChessMatch;

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