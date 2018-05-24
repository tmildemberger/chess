#ifndef CHESS_BASE_H
#define CHESS_BASE_H

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

#define WHITE 0
#define BLACK 1

#define TABLE_WIDTH 8
#define TABLE_HEIGHT 8

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
 * struct node --- ChessNode
 * isso serve para fazer
 * listas de peças
 */
typedef struct node{
    ChessPiece *p;
    struct node *nxt;
} ChessNode;

/**
 * struct game --- ChessBoard
 * essa é a estrutura principal,
 * que sabe qual é o tamanho do
 * tabuleiro, e mantém o registro
 * de quais peças estão vivas ou não
 */
typedef struct board{
    ChessNode alive_head;
    ChessNode not_alive_head;
    int board_width;
    int board_height;

} ChessBoard;

typedef struct jogada{

} ChessPlay;

ChessBoard* chess_new_game(void);
ChessPiece* chess_new_piece(char row, char col, char name, char team);

ChessPiece* chess_piece_in_pos(ChessBoard *play, char row, char col);

void chess_game_over(ChessBoard* play);
void chess_destroy_piece(ChessPiece* pic);

#endif