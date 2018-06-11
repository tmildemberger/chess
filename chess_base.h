#ifndef CHESS_BASE_H
#define CHESS_BASE_H

typedef enum pieces_type {  
    PAWN = 0,
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
    PROMOTION_MOVE
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
    signed char heading;
    unsigned char team : 7;
    unsigned char alive : 1;
    unsigned int movs;
    PiecesType type;
} ChessPiece;

typedef struct move{
    ChessSquare from;
    ChessSquare to;
    MoveType type;
    PiecesType targetType;
} ChessMove;

typedef struct visual_piece {
    ChessPiece *piece;
    int x;
    int y;
} ChessVisualPiece;

typedef struct move_list{
    struct move_list *nxt;
    unsigned int used_size;
    unsigned int allocated_size;
    ChessMove moves[]; 
} ChessMoveList;

typedef struct piece_list{
    struct piece_list *nxt;
    unsigned int used_size;
    unsigned int allocated_size;
    ChessPiece *pieces[]; 
} ChessPieceList;

typedef struct vpiece_list{
    struct vpiece_list *nxt;
    unsigned int used_size;
    unsigned int allocated_size;
    ChessVisualPiece *vpieces[]; 
} ChessVPieceList;

/**
 * struct game --- ChessBoard
 * essa é a estrutura principal,
 * que sabe qual é o tamanho do
 * tabuleiro, e mantém o registro
 * de quais peças estão vivas ou não
 */
typedef struct board{
    ChessPieceList *pieces;
    unsigned short board_width;
    unsigned short board_height;
    unsigned short movements;
    unsigned short turn;
} ChessBoard;

typedef struct match{
    ChessBoard board;
    ChessMoveList *record;
} ChessMatch;

ChessMatch* chess_new_game(void);
ChessPiece* chess_new_piece(char col, char row, PiecesType name, char team);

ChessPiece* chess_piece_in_pos(ChessMatch *play, char col, char row);
ChessMove chess_create_move(ChessMatch *play, 
                            ChessPiece *piece,
                            ChessSquare toSquare,
                            PiecesType targetType);
int chess_piece_cant_move(ChessMatch *play, ChessPiece *piece);
MoveType chess_analize_move(ChessMatch *play, 
                            ChessPiece *piece,
                            ChessMove move);
void chess_apply_move(ChessMatch *play, ChessMove *move);
int chess_squares_satifies(ChessMatch *play, ChessSquare fromSquare,
                            ChessSquare toSquare, 
                            int (*func)(ChessMatch*, ChessSquare));
int chess_empty_square(ChessMatch* play, ChessSquare square);
int chess_safe_square(ChessMatch* play, ChessSquare square);
int chess_same_square(ChessSquare sq1, ChessSquare sq2);
int chess_square_outside_board(ChessMatch *play, ChessSquare square);
int chess_not_in_your_turn(ChessMatch *play, ChessPiece *piece);
int chess_friendly_fire(ChessMatch *play, 
                        ChessPiece *attacker, 
                        ChessPiece *target);

void chess_game_over(ChessMatch *play);
void chess_destroy_piece(ChessPiece *pic);
void chess_destroy_move(ChessMove *move);

MoveType chess_no_special(ChessMatch*, ChessPiece*, ChessMove);
MoveType chess_en_passant(ChessMatch*, ChessPiece*, ChessMove);
MoveType chess_castling_move(ChessMatch*, ChessPiece*, ChessMove);
MoveType chess_promotion_move(ChessMatch*, ChessPiece*, ChessMove);
MoveType chess_pawn_specials(ChessMatch*, ChessPiece*, ChessMove);

MoveType (*chess_special_moves[])(ChessMatch*, ChessPiece*, ChessMove) = {
    chess_pawn_specials,
    chess_no_special,
    chess_no_special,
    chess_no_special,
    chess_no_special,
    chess_castling_move
};

int chess_pawn_ok_capture(ChessMatch*, ChessPiece*, ChessMove);
int chess_pawn_ok_normal(ChessMatch*, ChessPiece*, ChessMove);
int chess_rook_ok_move(ChessMatch*, ChessPiece*, ChessMove);
int chess_knight_ok_move(ChessMatch*, ChessPiece*, ChessMove);
int chess_bishop_ok_move(ChessMatch*, ChessPiece*, ChessMove);
int chess_queen_ok_move(ChessMatch*, ChessPiece*, ChessMove);
int chess_king_ok_move(ChessMatch*, ChessPiece*, ChessMove);

int (*chess_normal_moves[])(ChessMatch*, ChessPiece*, ChessMove) = {
    chess_pawn_ok_normal,
    chess_rook_ok_move,
    chess_knight_ok_move,
    chess_bishop_ok_move,
    chess_queen_ok_move,
    chess_king_ok_move
};

int (*chess_capture_moves[])(ChessMatch*, ChessPiece*, ChessMove) = {
    chess_pawn_ok_capture,
    chess_rook_ok_move,
    chess_knight_ok_move,
    chess_bishop_ok_move,
    chess_queen_ok_move,
    chess_king_ok_move
};

#endif