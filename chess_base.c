#include "chess_base.h"
#include "chess_error.h"

#include <stdlib.h>

/**
 * Primeira função a ser chamada - cria um novo jogo
 */
ChessGame* chess_new_game(){
    /**
     * por causa do calloc, o 'nxt' do 'not_alive_head' do
     * new_match já é inicializado com NULL (lista vazia)
     */
    ChessGame *new_match = calloc(1, sizeof(ChessGame));
    if (new_match == NULL){
        chess_error(ALLOC_ERROR);
    }

    /**
     * pelo menos por enquanto o tamanho do
     * tabuleiro está fixo em oito por oito
     */
    new_match->table_width = TABLE_WIDTH;
    new_match->table_height = TABLE_HEIGHT;

    char pieces[16] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,
                        PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN};
    
    ChessNode *last_node = &(new_match->alive_head);
    ChessNode *new_node = NULL;
    int i;
    for (i = 0; i < 16; i++){
        /**
         * por causa do calloc, o 'nxt' do new_node
         * já é inicializado com NULL
         */
        new_node = calloc(1, sizeof(ChessNode));
        if (new_node == NULL){
            chess_error(ALLOC_ERROR);
        }

        /**
         * fazemos o nó anterior apontar para o novo nó
         */
        last_node->nxt = new_node;

        
        new_node->p = chess_new_piece(  i / 8, 
                                        i % 8, 
                                        pieces[i], 
                                        WHITE);

        last_node = new_node;
    }

    for (i = 0; i < 16; i++){
        /**
         * por causa do calloc, o 'nxt' do new_node
         * já é inicializado com NULL
         */
        new_node = calloc(1, sizeof(ChessNode));
        if (new_node == NULL){
            chess_error(ALLOC_ERROR);
        }

        /**
         * fazemos o nó anterior apontar para o novo nó
         */
        last_node->nxt = new_node;

        /**
         * isso faz com que as novas peças sejam colocadas
         * na parte de cima do tabuleiro
         */
        new_node->p = chess_new_piece(  new_match->table_height - i / 8 - 1, 
                                        i % 8 - 1, 
                                        pieces[i], 
                                        BLACK);

        /**
         * o novo nó se torna o anterior para a próxima iteração
         */
        last_node = new_node;
    }

    return new_match;
}

/**
 * cria uma peça com as informações enviadas
 */
ChessPiece* chess_new_piece(char row, char col, char name, char team){
    ChessPiece *new_piece = calloc(1, sizeof(ChessPiece));
    if (new_piece == NULL){
        chess_error(ALLOC_ERROR);
    }

    new_piece->row = row;
    new_piece->column = col;
    new_piece->name = name;
    new_piece->team = team;
    /**
     * não é necessário inicializar a contagem de movimentos
     * da peça pois o calloc já inicializa tudo com zero
     */

    return new_piece;
}

ChessPiece* chess_piece_in_pos(ChessGame *play, char row, char col){
    ChessNode *current = play->alive_head.nxt;
    while (current != NULL){
        if (current->p->row == row && current->p->column == col){
            return current->p;
        }
        current = current->nxt;
    }
    return NULL;
}

/**
 * destrói o jogo todo
 */
void chess_game_over(ChessGame* play){
    /**
     * destruir todos os nós e peças ainda vivas
     */
    ChessNode *current = play->alive_head.nxt;
    ChessNode *next = NULL;
    while (current != NULL){
        /**
         * desde já guarda o endereço do próximo nó
         */
        next = current->nxt;

        /**
         * sempre bom limpar o valor dos ponteiros
         * antes de liberar a memória
         */
        current->nxt = NULL;

        /**
         * destrói peça e nó correspondente
         */
        chess_destroy_piece(current->p);
        free(current);

        /**
         * fazer o mesmo para o próximo até que
         * acabe a lista (marcado por NULL)
         */
        current = next;
    }
    /**
     * destruir todos os nós e peças não vivas
     * mesma coisa de antes, só que com o 'not_alive_head'
     */

    current = play->not_alive_head.nxt;
    next = NULL;
    while (current != NULL){
        /**
         * desde já guarda o endereço do próximo nó
         */
        next = current->nxt;

        /**
         * sempre bom limpar o valor dos ponteiros
         * antes de liberar a memória
         */
        current->nxt = NULL;

        /**
         * destrói peça e nó correspondente
         */
        chess_destroy_piece(current->p);
        free(current);

        /**
         * fazer o mesmo para o próximo até que
         * acabe a lista (marcado por NULL)
         */
        current = next;
    }

    /**
     * destruir o próprio ChessGame
     */
    free(play);
}

/**
 * somente destrói a peça
 */
void chess_destroy_piece(ChessPiece* pic){
    free(pic);
}
