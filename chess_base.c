#include "chess_base.h"
#include "chess_error.h"

#include <stdlib.h>

/**
 * Primeira função a ser chamada - cria um novo jogo
 */
ChessBoard* chess_new_game(void){
    /**
     * por causa do calloc, o 'nxt' do 'not_alive_head' do
     * new_board já é inicializado com NULL (lista vazia)
     */
    ChessBoard *new_board = calloc(1, sizeof(ChessBoard));
    if (new_board == NULL){
        chess_error(ALLOC_ERROR);
    }

    /**
     * pelo menos por enquanto o tamanho do
     * tabuleiro está fixo em oito por oito
     */
    new_board->board_width = TABLE_WIDTH;
    new_board->board_height = TABLE_HEIGHT;

    char pieces[16] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,
                        PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN};
    
    ChessNode *last_node = &(new_board->alive_head);
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

        
        new_node->piece = chess_new_piece(  i / 8, 
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
        new_node->piece = chess_new_piece(  new_board->board_height - i / 8 - 1, 
                                            i % 8, 
                                            pieces[i], 
                                            BLACK);

        /**
         * o novo nó se torna o anterior para a próxima iteração
         */
        last_node = new_node;
    }

    return new_board;
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

/**
 * retorna a peça na posição enviada, e NULL
 * se não houver peça nessa casa
 */
ChessPiece* chess_piece_in_pos(ChessBoard *play, char row, char col){
    ChessNode *current = play->alive_head.nxt;
    while (current != NULL){
        if (current->piece->row == row && current->piece->column == col){
            return current->piece;
        }
        current = current->nxt;
    }
    return NULL;
}

/**
 * recebe uma jogada e retorna VALID_MOVE se válida
 * ou INVALID_MOVE se inválida, levando em conta a 
 * situação do tabuleiro que também recebe
 */
int chess_is_valid_move(ChessBoard *play, ChessMove *move){
    /**
     * @todo: função pra ver se a peça pode se mover
     * (por enquanto só retorna sim), função pra ver se 
     * o movimento pode ser normal (se sim e se for um peão,
     * ver se ele chegou à última casa), funções pra ver se
     * o movimento pode ser especial
     */
    move->moveType = NORMAL_MOVE;
    return VALID_MOVE;
}

/**
 * no tabuleiro enviado, realiza uma jogada que se
 * presume ser válida
 */
void chess_apply_move(ChessBoard *play, ChessMove *move){
    ChessPiece *piece = chess_piece_in_pos(play, move->fromRow, move->fromCol);
    /**
     * para cada tipo de movimento algo diferente será feito.
     * serão chamadas funções ou vai ser tudo aqui?
     */
    if (move->moveType == NORMAL_MOVE){
        piece->movs++;
        piece->column = move->toCol;
        piece->row = move->toRow;
    }
}

/**
 * destrói o jogo todo
 */
void chess_game_over(ChessBoard* play){
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
        chess_destroy_piece(current->piece);
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
        chess_destroy_piece(current->piece);
        free(current);

        /**
         * fazer o mesmo para o próximo até que
         * acabe a lista (marcado por NULL)
         */
        current = next;
    }

    /**
     * destruir o próprio ChessBoard
     */
    free(play);
}

/**
 * somente destrói a peça
 */
void chess_destroy_piece(ChessPiece* pic){
    free(pic);
}
