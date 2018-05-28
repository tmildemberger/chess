#include "chess_base.h"
#include "chess_error.h"

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

        
        new_node->piece = chess_new_piece(  i % 8, 
                                            i / 8, 
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
        new_node->piece = chess_new_piece(  i % 8, 
                                            new_board->board_height - i / 8 - 1, 
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
ChessPiece* chess_new_piece(char col, char row, char name, char team){
    ChessPiece *new_piece = calloc(1, sizeof(ChessPiece));
    if (new_piece == NULL){
        chess_error(ALLOC_ERROR);
    }

    new_piece->column = col;
    new_piece->row = row;
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
ChessPiece* chess_piece_in_pos(ChessBoard *play, char col, char row){
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
ChessMove* chess_create_move(ChessBoard *play, ChessPiece *piece,
                                            char toCol, char toRow){
    /**
     * @todo: função pra ver se a peça pode se mover
     * (por enquanto só retorna sim), função pra ver se 
     * o movimento pode ser normal (se sim e se for um peão,
     * ver se ele chegou à última casa), funções pra ver se
     * o movimento pode ser especial
     */
    ChessMove *move = calloc(1, sizeof(ChessMove));
    if (move == NULL){
        chess_error(ALLOC_ERROR);
    }
    if (piece == NULL 
        || chess_piece_cant_move(play, piece) 
        || (toCol == piece->column && toRow == piece->row)
        || toCol < 0 || toCol > 7
        || toRow < 0 || toRow > 7
        || piece->team != play->turn){
        move->moveType = INVALID_MOVE;
        return move;
    }
    move->fromCol = piece->column;
    move->fromRow = piece->row;
    move->toCol = toCol;
    move->toRow = toRow;
    move->moveType = chess_analize_move(play, piece, toCol, toRow);
    return move;
}

/**
 * função retorna 1 se a peça deixa o rei
 * em cheque ao se mover
 */
int chess_piece_cant_move(ChessBoard *play, ChessPiece *piece){
    return 0;
}

int chess_analize_move(ChessBoard *play, ChessPiece *piece,
                                    char toCol, char toRow){
    int dx = toCol - piece->column;
    int dy = toRow - piece->row;
    int type = UNKNOWN_MOVE;
    ChessPiece *target = chess_piece_in_pos(play, toCol, toRow);
    ChessPiece *temp_en_passant = chess_piece_in_pos(play, toCol, toRow-1);
    switch (piece->name){
        case PAWN:
            if (dx == 0 && dy > 0 && dy <= 2 ){
                if (dy == 2){
                    if (piece->movs == 0){
                        type = NORMAL_MOVE;
                    } else {
                        type = INVALID_MOVE;
                    }
                } else {
                    if (target == NULL){
                        type = NORMAL_MOVE;
                    } else {
                        type = INVALID_MOVE;
                    }
                }
            } else if (dy == 1 && piece->name == WHITE && (dx == -1 || dx == +1)){
                // if (temp_en_passant != NULL
                //     && temp_en_passant->name == PAWN
                //     && temp_en_passant->movs == 1
                //     && temp_en_passant->row == play->board_height-3){
                //     type =
                // }
                if (target == NULL){
                    type = INVALID_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        type = CAPTURE_MOVE;
                    }
                }
            } else if (dy == -1 && piece->name == BLACK && (dx == -1 || dx == +1)){
                if (target == NULL){
                    type = INVALID_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        type = CAPTURE_MOVE;
                    }
                }
            }
            break;
        case ROOK:
            if (dx == 0 || dy == 0){
                if (target == NULL){
                    type = NORMAL_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        type = CAPTURE_MOVE;
                    }
                }
            }
            break;
        case KNIGHT:
            if (dx + dy == 3 && dx * dy == 2){
                if (target == NULL){
                    type = NORMAL_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        type = CAPTURE_MOVE;
                    }
                }
            }
            break;
        case BISHOP:
            if (dx == dy){
                if (target == NULL){
                    type = NORMAL_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        type = CAPTURE_MOVE;
                    }
                }
            }
            break;
        case QUEEN:
            if (dx == 0 || dy == 0 || dx == dy){
                if (target == NULL){
                    type = NORMAL_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        type = CAPTURE_MOVE;
                    }
                }
            }
            break;
        case KING:
            if (dx <= 1 && dy <= 1){
                if (target == NULL){
                    type = NORMAL_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        type = CAPTURE_MOVE;
                    }
                }
            }
            break;
    }
    return type;
}

/**
 * no tabuleiro enviado, realiza uma jogada que se
 * presume ser válida
 */
void chess_apply_move(ChessBoard *play, ChessMove *move){
    ChessPiece *piece = chess_piece_in_pos(play, move->fromCol, move->fromRow);
    ChessPiece *target = chess_piece_in_pos(play, move->toCol, move->toRow);
    /**
     * para cada tipo de movimento algo diferente será feito.
     * serão chamadas funções ou vai ser tudo aqui?
     */
    switch (move->moveType){
        case NORMAL_MOVE:
            piece->movs++;
            piece->column = move->toCol;
            piece->row = move->toRow;
            break;
        case CAPTURE_MOVE:
            piece->movs++;
            piece->column = move->toCol;
            piece->row = move->toRow;
            /**
             * also remove target from alive list
             */

            break;
        case CASTLING_MOVE:
            break;
        case EN_PASSANT_MOVE:
            break;
        case PROMOTION_MOVE:
            break;
        case PROMOTION_PIECE:
            break;
        default:
            break;
    }
}

/**
 * destrói o jogo todo
 */
void chess_game_over(ChessBoard *play){
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
void chess_destroy_piece(ChessPiece *pic){
    free(pic);
}

void chess_destroy_move(ChessMove *mov){
    free(mov);
}
