#include "chess_base.h"
#include "chess_piece_list.h"
#include "chess_move_list.h"

/**
 * Primeira função a ser chamada - cria um novo jogo
 */
ChessMatch* chess_new_game(void){
    /**
     * por causa do calloc, o 'nxt' do 'not_alive_head' do
     * new_match já é inicializado com NULL (lista vazia)
     */
    ChessMatch *new_match = calloc(1, sizeof(ChessMatch));
    if (new_match == NULL){
        chess_error(ALLOC_ERROR);
    }

    new_match->board.pieces = chess_create_new_piece_list();
    new_match->record = chess_create_move_list();

    /**
     * pelo menos por enquanto o tamanho do
     * tabuleiro está fixo em oito por oito
     */
    new_match->board.board_width = TABLE_WIDTH;
    new_match->board.board_height = TABLE_HEIGHT;

    PiecesType pieces[16] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,
                        PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN};
    
    ChessPiece *new_piece = NULL;
    int i;
    for (i = 0; i < 16; i++){
        
        new_piece = chess_new_piece(        i % 8, 
                                            i / 8, 
                                            pieces[i], 
                                            WHITE);

        chess_append_piece(new_match->board.pieces, new_piece);
    }

    for (i = 0; i < 16; i++){
        
        new_piece = chess_new_piece(        i % 8, 
                                            new_match->board.board_height - i / 8 - 1, 
                                            pieces[i], 
                                            BLACK);

        chess_append_piece(new_match->board.pieces, new_piece);        
    }

    return new_match;
}

/**
 * cria uma peça com as informações enviadas
 */
ChessPiece* chess_new_piece(char col, char row, PiecesType name, char team){
    ChessPiece *new_piece = calloc(1, sizeof(ChessPiece));
    if (new_piece == NULL){
        chess_error(ALLOC_ERROR);
    }

    new_piece->pos = (ChessSquare) { col, row };
    new_piece->type = name;
    new_piece->alive = 1;
    new_piece->team = team;
    new_piece->heading = team == WHITE ? 1 : -1;
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
ChessPiece* chess_piece_in_pos(ChessMatch *play, char col, char row){
    return chess_find_piece_in_square(play->board.pieces, 
                                      (ChessSquare){ col, row });
}

/**
 * recebe uma jogada e retorna VALID_MOVE se válida
 * ou INVALID_MOVE se inválida, levando em conta a 
 * situação do tabuleiro que também recebe
 */
ChessMove* chess_create_move(ChessMatch *play, ChessPiece *piece,
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
        || (toCol == piece->pos.col && toRow == piece->pos.row)
        || toCol < 0 || toCol > 7
        || toRow < 0 || toRow > 7
        || piece->team != play->board.turn){
        move->type = INVALID_MOVE;
        return move;
    }
    move->from = piece->pos;
    move->to = (ChessSquare) { toCol, toRow };
    move->type = chess_analize_move(play, piece, toCol, toRow);
    return move;
}

/**
 * função retorna 1 se a peça deixa o rei
 * em cheque ao se mover
 */
int chess_piece_cant_move(ChessMatch *play, ChessPiece *piece){
    return 0;
}

MoveType chess_analize_move(ChessMatch *play, ChessPiece *piece,
                                    char toCol, char toRow){
    int dx = toCol - piece->pos.col;
    int dy = toRow - piece->pos.row;
    int type = UNKNOWN_MOVE;
    ChessPiece *target = chess_piece_in_pos(play, toCol, toRow);
    ChessPiece *temp_en_passant = chess_piece_in_pos(play, toCol, toRow-piece->heading);
    piece->alive = 0;
    switch (piece->type){
        case PAWN:
            if (dx == 0 && dy >= -2 && dy <= 2 ){
                if (dy == 2 || dy == -2){
                    if (piece->movs == 0){
                        if (chess_squares_satifies(play, piece->pos, (ChessSquare){toCol, toRow}, chess_empty_square))
                            type = NORMAL_MOVE;
                        else 
                            type = INVALID_MOVE;
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
            } else if (dy == 1 && piece->team == WHITE && (dx == -1 || dx == +1)){
                if (target == NULL){
                    // if (temp_en_passant != NULL
                    //     && temp_en_passant->name == PAWN
                    //     && temp_en_passant->movs == 1
                    //     && temp_en_passant->row == play->board_height-4){
                    //     type = EN_PASSANT_MOVE;
                    // } else {
                        type = INVALID_MOVE;
                    // }
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        type = CAPTURE_MOVE;
                    }
                }
            } else if (dy == -1 && piece->team == BLACK && (dx == -1 || dx == +1)){
                if (target == NULL){
                    // if (temp_en_passant != NULL
                    //     && temp_en_passant->name == PAWN
                    //     && temp_en_passant->movs == 1
                    //     && temp_en_passant->row == 3){
                    //     type = EN_PASSANT_MOVE;
                    // } else {
                        type = INVALID_MOVE;
                    // }
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
                    /**
                     * talvez a função saiba andar sozinha
                     * se o x ainda é diferente, continuar mudando,
                     * se já é igual, mudar só o outro
                     * o mesmmo para o y
                     */
                    if (chess_squares_satifies(play, piece->pos, (ChessSquare){toCol, toRow}, chess_empty_square))
                        type = NORMAL_MOVE;
                    else 
                        type = INVALID_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        if (chess_squares_satifies(play, piece->pos, (ChessSquare){toCol, toRow}, chess_empty_square))
                            type = CAPTURE_MOVE;
                        else 
                            type = INVALID_MOVE;
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
                    if (chess_squares_satifies(play, piece->pos, (ChessSquare){toCol, toRow}, chess_empty_square))
                        type = NORMAL_MOVE;
                    else 
                        type = INVALID_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        if (chess_squares_satifies(play, piece->pos, (ChessSquare){toCol, toRow}, chess_empty_square))
                            type = CAPTURE_MOVE;
                        else 
                            type = INVALID_MOVE;
                    }
                }
            }
            break;
        case QUEEN:
            if (dx == 0 || dy == 0 || dx == dy){
                if (target == NULL){
                    if (chess_squares_satifies(play, piece->pos, (ChessSquare){toCol, toRow}, chess_empty_square))
                        type = NORMAL_MOVE;
                    else 
                        type = INVALID_MOVE;
                } else {
                    if (target->team == piece->team){
                        type = INVALID_MOVE;
                    } else {
                        if (chess_squares_satifies(play, piece->pos, (ChessSquare){toCol, toRow}, chess_empty_square))
                            type = CAPTURE_MOVE;
                        else 
                            type = INVALID_MOVE;
                    }
                }
            }
            break;
        case KING:
            if (dy == 0 && (dx == 2 || dx == -2)){
                if (target == NULL){
                    ChessPiece *rook = chess_piece_in_pos(play, 
                                        dx == 2 ? play->board.board_width-1 : 0, piece->pos.row);
                    if (rook != NULL){
                        if (rook->movs == 0 && piece->movs == 0){
                            if (chess_squares_satifies(play, piece->pos, (ChessSquare){toCol, toRow}, chess_empty_square)){
                                rook->alive = 0;
                                if (chess_squares_satifies(play, rook->pos, dx == 2 ? ((ChessSquare){toCol-1, toRow}) : ((ChessSquare){toCol+1, toRow}), chess_empty_square)){
                                    if (chess_safe_square(play, (ChessSquare){toCol, toRow})
                                        && chess_squares_satifies(play, piece->pos,
                                                                (ChessSquare){toCol, toRow}, chess_safe_square)){
                                        type = CASTLING_MOVE;
                                    }
                                }
                                rook->alive = 1;
                            }
                        }
                    }
                }
            } else if (dx <= 1 && dy <= 1){
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
    piece->alive = 1;
    return type;
}

/**
 * no tabuleiro enviado, realiza uma jogada que se
 * presume ser válida
 */
void chess_apply_move(ChessMatch *play, ChessMove *move){
    ChessPiece *piece = chess_piece_in_pos(play, move->from.col, move->from.row);
    ChessPiece *target = chess_piece_in_pos(play, move->to.col, move->to.row);
    /**
     * para cada tipo de movimento algo diferente será feito.
     * serão chamadas funções ou vai ser tudo aqui?
     */
    if (move->type >= NORMAL_MOVE && move->type <= PROMOTION_MOVE){
        play->board.movements++;
        play->board.turn++;
        play->board.turn %= 2;
    }
    switch (move->type){
        case NORMAL_MOVE:
            piece->movs++;
            piece->pos = move->to;
            break;
        case CAPTURE_MOVE:{
            // ChessNode *node = chess_list_find_piece(play->alive_head, target);
            piece->movs++;
            piece->pos = move->to;
            target->alive = 0;
            break;
        }
        case CASTLING_MOVE:
            break;
        case EN_PASSANT_MOVE:
            break;
        case PROMOTION_MOVE:
            break;
        default:
            break;
    }
}

/**
 * destrói o jogo todo
 */
void chess_game_over(ChessMatch *play){
    chess_destroy_piece_list(play->board.pieces);
    chess_destroy_move_list(play->record);
    /**
     * destruir o próprio ChessMatch
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

int chess_empty_square(ChessMatch* play, ChessSquare square){
    return (chess_piece_in_pos(play, square.col, square.row) == NULL);
}

int chess_safe_square(ChessMatch* play, ChessSquare square){
    return 1;
}

int chess_squares_satifies(ChessMatch *play, ChessSquare fromSquare,
                            ChessSquare toSquare, 
                            int (*func)(ChessMatch*, ChessSquare)){
    if (fromSquare.col == toSquare.col && fromSquare.row == toSquare.row)
        return 1;
    if (!func(play, fromSquare))
        return 0;
    int incX = 0;
    int incY = 0;
    if (toSquare.col - fromSquare.col < 0)
        incX = -1;
    else if (toSquare.col - fromSquare.col > 0)
        incX = 1;
    if (toSquare.row - fromSquare.row < 0)
        incY = -1;
    else if (toSquare.row - fromSquare.row > 0)
        incY = 1;
    return chess_squares_satifies(play, (ChessSquare){fromSquare.col+incX,
                                                      fromSquare.row+incY},
                                    toSquare, func);
}