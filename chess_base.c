#include "chess_base.h"
#include "chess_piece_list.h"
#include "chess_move_list.h"

#include <time.h>       //time
#include <stdlib.h>     //srand, rand e abs

MoveType (*chess_special_moves[])(ChessMatch*, ChessPiece*, ChessMove) = {
    chess_pawn_specials,
    chess_no_special,
    chess_no_special,
    chess_no_special,
    chess_no_special,
    chess_castling_move
};

int (*chess_normal_moves[])(ChessPiece*, int, int) = {
    chess_pawn_ok_normal,
    chess_rook_ok_move,
    chess_knight_ok_move,
    chess_bishop_ok_move,
    chess_queen_ok_move,
    chess_king_ok_move
};

int (*chess_capture_moves[])(ChessPiece*, int, int) = {
    chess_pawn_ok_capture,
    chess_rook_ok_move,
    chess_knight_ok_move,
    chess_bishop_ok_move,
    chess_queen_ok_move,
    chess_king_ok_move
};

int (*chess_apply_moves[])(ChessMatch*, ChessPiece*, ChessMove*) = {
    chess_apply_invalid_move,
    chess_apply_normal_move,
    chess_apply_capture_move,
    chess_apply_castling_move,
    chess_apply_en_passant_move,
    chess_apply_promotion_move,
    chess_apply_promotion_capture_move
};

int (*chess_pieces_jumps[])(ChessMatch*, ChessSquare, ChessSquare, CheckType) = {
    chess_empty_squares,
    chess_empty_squares,
    chess_ignore_squares,
    chess_empty_squares,
    chess_empty_squares,
    chess_empty_squares
};

int (*chess_undo_moves[])(ChessMatch*, ChessPiece*, ChessMove) = {
    chess_undo_invalid_move,
    chess_undo_normal_move,
    chess_undo_capture_move,
    chess_undo_castling_move,
    chess_undo_en_passant_move,
    chess_undo_promotion_move,
    chess_undo_promotion_capture_move
};

/**
 * Primeira função a ser chamada - cria um novo jogo
 */
ChessMatch* chess_new_game(void){
    srand(time(NULL));
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
ChessPiece* chess_piece_in_pos(ChessMatch *play, ChessSquare square){
    return chess_find_piece_in_square(play->board.pieces, 
                                      square);
}

/**
 * cria uma jogada com tudo dentro
 */
ChessMove chess_create_move(ChessMatch *play, 
                            ChessPiece *piece,
                            ChessSquare toSquare){

    ChessPiece *target = chess_piece_in_pos(play, toSquare);
    ChessMove move = {{0, 0}, {0, 0}, 0, 0, 0};
    if (piece == NULL ||
        //chess_piece_cant_move(play, piece) ||
        chess_square_outside_board(play, toSquare) ||
        chess_same_square(piece->pos, toSquare) ||
        chess_not_in_your_turn(play, piece) ||
        chess_friendly_fire(play, piece, target) ){
        move.type = INVALID_MOVE;
        return move;
    }
    move.from = piece->pos;
    move.to = toSquare;
    move.type = chess_analize_move(play, piece, move);
    if (move.type != INVALID_MOVE){
        chess_apply_move(play, &move);
        if (chess_in_check(play, piece->team)){
            move.type = INVALID_MOVE;
        }
        chess_undo_last_move(play);
    }
    return move;
}

/**
 * função retorna 1 se a peça deixa o rei
 * em cheque ao se mover
 */
int chess_piece_cant_move(ChessMatch *play, ChessPiece *piece){
    return 0;
}

MoveType chess_analize_move(ChessMatch *play, 
                            ChessPiece *piece,
                            ChessMove move){
    /**
     * checar se é possível um movimento especial
     * depois ver que o que faz para movimentos normais 
     * e de captura...
     */
    MoveType type;
    type = chess_special_check(play, piece, move);
    if (type != INVALID_MOVE) return type;

    return chess_other_check(play, piece, move);
}

MoveType chess_no_special(ChessMatch *play, 
                          ChessPiece *piece, 
                          ChessMove move){
    return INVALID_MOVE;
}

MoveType chess_en_passant(ChessMatch *play, 
                          ChessPiece *piece, 
                          ChessMove move){
    ChessMove lastMove = chess_last_move(play->record);
    ChessPiece *otherPawn = chess_piece_in_pos(play, lastMove.to);

    int dx = move.to.col - piece->pos.col;
    int dy = move.to.row - piece->pos.row;
    if (//move.targetType == PAWN &&
        otherPawn != NULL &&
        otherPawn->type == PAWN &&
        otherPawn->movs == 1 &&
        (lastMove.to.row - lastMove.from.row == -2 ||
        lastMove.to.row - lastMove.from.row == 2) &&
        chess_same_square(otherPawn->pos, 
                          (ChessSquare){ move.to.col, piece->pos.row }) &&
        chess_pawn_ok_capture(piece, dx, dy)) //movimento é igual
        return EN_PASSANT_MOVE;
    else 
        return INVALID_MOVE;
}

MoveType chess_castling_move(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove move){
    unsigned short lastCol = move.to.col - move.from.col > 0 ? 
                             play->board.board_width - 1 : 
                             0;
    ChessPiece *rook = chess_piece_in_pos(play, 
                                          (ChessSquare){ lastCol, move.to.row });
    if (//move.targetType == KING &&
        move.to.row == move.from.row &&
        (move.to.col - move.from.col == 2 ||
        move.to.col - move.from.col == -2) &&
        rook != NULL &&
        rook->team == piece->team && //talvez desnecessário
        rook->type == ROOK &&
        piece->movs == 0 &&
        rook->movs == 0 &&
        chess_empty_squares(play, piece->pos, rook->pos, NO_LAST) &&
        chess_safe_squares(play, piece->pos, move.to, LAST)){
        return CASTLING_MOVE;
    } else
        return INVALID_MOVE;
}

MoveType chess_promotion_move(ChessMatch *play, 
                              ChessPiece *piece, 
                              ChessMove move){
    unsigned short lastRow = piece->heading > 0 ? 
                             play->board.board_height - 1 : 
                             0;
    int dx = move.to.col - piece->pos.col;
    int dy = move.to.row - piece->pos.row;
    if (move.to.row == lastRow && 
        chess_pawn_ok_normal(piece, dx, dy))
        return PROMOTION_NORMAL_MOVE;
    else 
        return INVALID_MOVE;
}

MoveType chess_promotion_capture_move(ChessMatch *play, 
                                      ChessPiece *piece, 
                                      ChessMove move){
    unsigned short lastRow = piece->heading > 0 ? 
                             play->board.board_height - 1 : 
                             0;
    int dx = move.to.col - piece->pos.col;
    int dy = move.to.row - piece->pos.row;
    if (move.to.row == lastRow && 
        chess_piece_in_pos(play, move.to) &&
        chess_pawn_ok_capture(piece, dx, dy))
        return PROMOTION_CAPTURE_MOVE;
    else 
        return INVALID_MOVE;
}

MoveType chess_pawn_specials(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove move){
    MoveType type = chess_en_passant(play, piece, move);
    if (type != INVALID_MOVE) return type;
    type = chess_promotion_move(play, piece, move);
    if (type != INVALID_MOVE) return type;
    return chess_promotion_capture_move(play, piece, move);
}

MoveType chess_special_check(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove move){
    return chess_special_moves[piece->type](play, piece, move);
}

MoveType chess_other_check(ChessMatch *play, 
                           ChessPiece *piece, 
                           ChessMove move){
    ChessPiece *target = chess_piece_in_pos(play, move.to);
    int dx = move.to.col - piece->pos.col;
    int dy = move.to.row - piece->pos.row;
    if (target != NULL){
        return //move.targetType == target->type &&
               chess_pieces_jumps[piece->type](play, move.from, move.to, NO_LAST) &&
               chess_capture_moves[piece->type](piece, dx, dy) ? 
               CAPTURE_MOVE : 
               INVALID_MOVE;
    } else {
        return //move.targetType == piece->type &&
               chess_pieces_jumps[piece->type](play, move.from, move.to, LAST) &&
               chess_normal_moves[piece->type](piece, dx, dy) ?
               NORMAL_MOVE :
               INVALID_MOVE;
    }
}

/**
 * no tabuleiro enviado, realiza uma jogada que se
 * presume ser válida
 */
void chess_apply_move(ChessMatch *play, ChessMove *move){
    ChessPiece *piece = chess_piece_in_pos(play, move->from);
    if (chess_apply_moves[move->type](play, piece, move)){
        play->board.movements++;
        play->board.turn++;
        play->board.turn %= 2;
        chess_append_move(play->record, (*move));
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

int chess_empty_square(ChessMatch* play, ChessSquare square){
    return (chess_piece_in_pos(play, square) == NULL);
}
/*
int chess_safe_square(ChessMatch* play, ChessSquare square){
    ChessPiece *target = NULL;
    int delete_piece = 0;
    int is_safe = 0;
    if (chess_piece_in_pos(play, square) == NULL){
        target = chess_new_piece(square.col, 
                                 square.row, 
                                 PAWN, 
                                 (play->board.turn + 1) % 2);
        chess_append_piece(play->board.pieces, target);
        delete_piece = 1;
    }
    ChessMoveList *possible_moves = chess_possible_moves_to(play, square);
    //ignora en passant por enquanto
    if (possible_moves->used_size == 0){
        is_safe = 1;
    }
    chess_destroy_move_list(possible_moves);
    if (delete_piece){
        chess_destroy_last_piece(play->board.pieces);
    }
    return is_safe;
}
*/
int chess_same_square(ChessSquare sq1, ChessSquare sq2){
    return sq1.col == sq2.col && sq1.row == sq2.row;
}

int chess_square_outside_board(ChessMatch *play, ChessSquare square){
    return square.col >= play->board.board_width ||
           square.row >= play->board.board_height;
}

int chess_not_in_your_turn(ChessMatch *play, ChessPiece *piece){
    return piece->team != play->board.turn;
}

int chess_friendly_fire(ChessMatch *play, 
                        ChessPiece *attacker, 
                        ChessPiece *target){
    return target != NULL && attacker->team == target->team;
}

int chess_pawn_ok_capture(ChessPiece *piece, int dx, int dy){
    return (dx == 1 || dx == -1) && dy == piece->heading;
}

int chess_pawn_ok_normal(ChessPiece *piece, int dx, int dy){
    return dx == 0 && (
           (dy == (2*piece->heading) && piece->movs == 0) ||
           (dy == piece->heading)
           );
}

int chess_rook_ok_move(ChessPiece *piece, int dx, int dy){
    return dx == 0 || dy == 0;
}

int chess_knight_ok_move(ChessPiece *piece, int dx, int dy){
    /// trocado por abs()
    dx = abs(dx); //dx = dx>0 ? dx : -dx;
    dy = abs(dy); //dy = dy>0 ? dy : -dy;
    return dx + dy == 3 && dx * dy == 2;
}

int chess_bishop_ok_move(ChessPiece *piece, int dx, int dy){
    return dx == dy || dx == -dy;
}

int chess_queen_ok_move(ChessPiece *piece, int dx, int dy){
    return chess_rook_ok_move(piece, dx, dy) ||
           chess_bishop_ok_move(piece, dx, dy);
}

int chess_king_ok_move(ChessPiece *piece, int dx, int dy){
    dx = abs(dx); //dx = dx>0 ? dx : -dx;
    dy = abs(dy); //dy = dy>0 ? dy : -dy;
    return dx <= 1 && dy <= 1;
}

int chess_apply_invalid_move(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove *move){
    return 0;
}

int chess_apply_normal_move(ChessMatch *play, 
                            ChessPiece *piece, 
                            ChessMove *move){
    chess_put_piece_in(piece, move->to);
    return 1;
}

int chess_apply_capture_move(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove *move){
    ChessPiece *target = chess_piece_in_pos(play, move->to);
    target->alive = 0;
    move->targetType = target->type;
    chess_put_piece_in(piece, move->to);
    return 1;
}

int chess_apply_castling_move(ChessMatch *play, 
                              ChessPiece *piece, 
                              ChessMove *move){
    unsigned short lastCol = move->to.col - move->from.col > 0 ? 
                             play->board.board_width - 1 : 
                             0;
    chess_put_piece_in(
        chess_piece_in_pos(play, (ChessSquare){ lastCol, move->to.row }),
        (ChessSquare){ lastCol ? move->to.col-1 : move->to.row+1, move->to.row });
    chess_put_piece_in(piece, move->to);
    return 1;
}

int chess_apply_en_passant_move(ChessMatch *play, 
                                ChessPiece *piece, 
                                ChessMove *move){
    ChessSquare unlucky = { move->to.col, move->from.row };
    (chess_piece_in_pos(play, unlucky))->alive = 0;
    chess_put_piece_in(piece, move->to);
    return 1;
}

int chess_apply_promotion_move(ChessMatch *play, 
                               ChessPiece *piece, 
                               ChessMove *move){
    chess_put_piece_in(piece, move->to);
    piece->type = move->promotionType != PAWN ?
                  move->promotionType :
                  QUEEN;
    return 1;
}

int chess_apply_promotion_capture_move(ChessMatch *play, 
                                       ChessPiece *piece, 
                                       ChessMove *move){
    ChessPiece *target = chess_piece_in_pos(play, move->to);
    target->alive = 0;
    move->targetType = target->type;
    chess_put_piece_in(piece, move->to);
    piece->type = move->promotionType != PAWN ?
                  move->promotionType :
                  QUEEN;
    return 1;
}

void chess_put_piece_in(ChessPiece *piece, ChessSquare toSquare){
    piece->movs++;
    piece->pos = toSquare;
}

void chess_unput_piece_in(ChessPiece *piece, ChessSquare toSquare){
    piece->movs--;
    piece->pos = toSquare;
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

DirectionType chess_get_directions(ChessSquare from, ChessSquare to){
    DirectionType dir = NO;
    if (to.col > from.col) dir += RIGHT;
    else if (to.col < from.col) dir += LEFT;

    if (to.row > from.row) dir += UP;
    else if (to.row < from.row) dir += DOWN;
    return dir;
}

void chess_inc_square(ChessSquare *square, DirectionType dir){
    switch (dir){
        case UP:
            square->row++;
            break;
        case DOWN:
            square->row--;
            break;
        case RIGHT:
            square->col++;
            break;
        case LEFT:
            square->col--;
            break;
        case UP_RIGHT:
            square->row++;
            square->col++;
            break;
        case UP_LEFT:
            square->row++;
            square->col--;
            break;
        case DOWN_RIGHT:
            square->row--;
            square->col++;
            break;
        case DOWN_LEFT:
            square->row--;
            square->col--;
            break;
        case NO:
        default:
            break;
    }
}

int chess_empty_squares(ChessMatch *play, 
                        ChessSquare fromSquare,
                        ChessSquare toSquare,
                        CheckType check){
    DirectionType dir = chess_get_directions(fromSquare, toSquare);
    ChessSquare square = fromSquare;
    chess_inc_square(&square, dir);
    while (!chess_same_square(square, toSquare)){
        if (chess_piece_in_pos(play, square) != NULL) return 0;
        chess_inc_square(&square, dir);
    }
    if (check == LAST){
        return chess_piece_in_pos(play, square) == NULL;
    } else {
        return 1;
    }
}

int chess_safe_squares(ChessMatch *play, 
                       ChessSquare fromSquare,
                       ChessSquare toSquare,
                       CheckType check){
    DirectionType dir = chess_get_directions(fromSquare, toSquare);
    ChessSquare square = fromSquare;
    chess_inc_square(&square, dir);
    while (!chess_same_square(square, toSquare)){
        if (chess_safe_square(play, square) == 0) return 0;
        chess_inc_square(&square, dir);
    }
    if (check == LAST){
        return chess_safe_square(play, square);
    } else {
        return 1;
    }
}

int chess_ignore_squares(ChessMatch *play, 
                         ChessSquare fromSquare,
                         ChessSquare toSquare,
                         CheckType check){
    return 1;
}

ChessMoveList* chess_piece_possible_moves(ChessMatch *play,
                                          ChessPiece *piece){

    ChessMoveList *possible_moves = chess_create_move_list();
    if (chess_not_in_your_turn(play, piece)){
        return possible_moves;
    }
    ChessMove move;
    int row, col;
    for (row = 0; row < play->board.board_height; row++){
        for (col = 0; col < play->board.board_width; col++){
            ChessSquare to = { col, row };
            
            //se mudar a maneira que o tipo da peça funciona
            //isso precisará ser mudado

            //^previsão correta. alterado:
            move = chess_create_move(play, piece, to);
            if (move.type != INVALID_MOVE){
                if (move.type == PROMOTION_NORMAL_MOVE ||
                    move.type == PROMOTION_CAPTURE_MOVE){
                    chess_promote_to(&move, KNIGHT);
                    chess_append_move(possible_moves, move);
                    chess_promote_to(&move, QUEEN);
                }
                chess_append_move(possible_moves, move);
            }
        }
    }
    return possible_moves;
}

ChessMoveList* chess_possible_moves_to(ChessMatch *play,
                                       ChessSquare to){
    
    ChessMoveList *possible_moves = chess_create_move_list();
    ChessPiece *piece;
    ChessMove move;
    int i = 0;
    for (piece = chess_piece_index(play->board.pieces, i);
         piece == NULL;
         piece = chess_piece_index(play->board.pieces, ++i)){

        if (piece->alive == 0 ||
            chess_piece_cant_move(play, piece) ||
            chess_not_in_your_turn(play, piece)){
            continue;
        }
        
        move = chess_create_move(play, piece, to);
        if (move.type != INVALID_MOVE){
            if (move.type == PROMOTION_NORMAL_MOVE ||
                move.type == PROMOTION_CAPTURE_MOVE){
                chess_promote_to(&move, KNIGHT);
                chess_append_move(possible_moves, move);
                chess_promote_to(&move, QUEEN);
            }
            chess_append_move(possible_moves, move);
        }
    }
    return possible_moves;
}
/*
int chess_can_be_reached(ChessMatch *play, ChessPiece *piece, ChessSquare to){
    ChessMoveList *possible_moves = chess_create_move_list();
    ChessPiece *piece;
    ChessMove move;
    int i = 0;
    int pc;
    for (piece = chess_piece_index(play->board.pieces, i);
         piece == NULL;
         piece = chess_piece_index(play->board.pieces, ++i)){
        
        ;
    }
}
*/

int chess_undo_invalid_move(ChessMatch *play, 
                            ChessPiece *piece, 
                            ChessMove move){
    return 0;
}

int chess_undo_normal_move(ChessMatch *play, 
                           ChessPiece *piece, 
                           ChessMove move){
    chess_unput_piece_in(piece, move.from);
    return 1;
}

int chess_undo_capture_move(ChessMatch *play, 
                            ChessPiece *piece, 
                            ChessMove move){
    (chess_find_not_alive_piece(play->board.pieces, 
                                move.to, 
                                move.targetType, 
                                (piece->team + 1) % 2))->alive = 1;
    chess_unput_piece_in(piece, move.from);
    return 1;
}

int chess_undo_castling_move(ChessMatch *play, 
                              ChessPiece *piece, 
                              ChessMove move){
    unsigned short lastCol = move.to.col - move.from.col > 0 ? 
                             play->board.board_width - 1 : 
                             0;
    chess_unput_piece_in(
        chess_piece_in_pos(play, 
        (ChessSquare){ lastCol ? move.to.col-1 : move.to.row+1, move.to.row }),
        (ChessSquare){ lastCol, move.to.row });
    chess_unput_piece_in(piece, move.from);
    return 1;
}

int chess_undo_en_passant_move(ChessMatch *play, 
                                ChessPiece *piece, 
                                ChessMove move){
    ChessSquare unlucky = { move.to.col, move.from.row };
    (chess_find_not_alive_piece(play->board.pieces,
                                unlucky,
                                move.targetType, // == PAWN
                                (piece->team + 1) % 2))->alive = 1;
    chess_unput_piece_in(piece, move.from);
    return 1;
}

int chess_undo_promotion_move(ChessMatch *play, 
                               ChessPiece *piece, 
                               ChessMove move){
    chess_unput_piece_in(piece, move.from);
    piece->type = PAWN;
    return 1;
}

int chess_undo_promotion_capture_move(ChessMatch *play, 
                                      ChessPiece *piece, 
                                      ChessMove move){
    (chess_find_not_alive_piece(play->board.pieces, 
                                move.to, 
                                move.targetType, 
                                (piece->team + 1) % 2))->alive = 1;
    chess_unput_piece_in(piece, move.from);
    piece->type = PAWN;
    return 1;
}

int chess_can_be_reached(ChessMatch *play, ChessPiece *piece, ChessSquare to){
    ChessMove move = { piece->pos, to, INVALID_MOVE, 0, 0 };
    ChessPiece *target = chess_piece_in_pos(play, to);
    if (piece == NULL ||
        chess_square_outside_board(play, to) ||
        chess_same_square(piece->pos, to) ||
        chess_friendly_fire(play, piece, target) ||
        chess_analize_move(play, piece, move) == INVALID_MOVE){
        return 0;
    }
    return 1;
}

void chess_undo_last_move(ChessMatch *play){
    ChessMove move = chess_last_move(play->record);
    chess_remove_last_move(play->record);
    ChessPiece *piece = chess_piece_in_pos(play, move.to);
    chess_undo_moves[move.type](play, piece, move);
    play->board.movements--;
    play->board.turn++;
    play->board.turn %= 2;
}

int chess_in_check(ChessMatch *play, unsigned char team){
    ChessPiece *king = chess_find_king(play->board.pieces, team);

    return !chess_safe_square(play, king->pos);
}

int chess_safe_square(ChessMatch* play, ChessSquare square){

    ChessPiece *target = NULL;
    int delete_piece = 0;
    int is_safe = 1;
    if (chess_piece_in_pos(play, square) == NULL){
        target = chess_new_piece(square.col, 
                                 square.row, 
                                 PAWN, 
                                 (play->board.turn));// + 1) % 2);
        chess_append_piece(play->board.pieces, target);
        delete_piece = 1;
    }

    ChessPiece *piece;
    int i = 0;
    //ignora en passant por enquanto
    for (piece = chess_piece_index(play->board.pieces, i);
         piece != NULL;
         piece = chess_piece_index(play->board.pieces, ++i)){
        
        if (piece->alive == 0) continue;
        if (chess_can_be_reached(play, piece, square)){
            is_safe = 0;
            break;
        }
    }
    
    if (delete_piece){
        chess_destroy_last_piece(play->board.pieces);
    }
    return is_safe;
}

ChessMoveList* chess_every_possible_move(ChessMatch *play){
    ChessMoveList *possible_moves = chess_create_move_list();
    ChessPiece *piece;
    int i = 0;
    for (piece = chess_piece_index(play->board.pieces, i);
         piece != NULL;
         piece = chess_piece_index(play->board.pieces, ++i)){
        
        if (piece->alive == 0) continue;
        chess_append_move_section(possible_moves, chess_piece_possible_moves(play, piece));
    }
    return possible_moves;
}

int chess_is_checkmate(ChessMatch *play){
    if (!chess_in_check(play, play->board.turn)) return 0;
    
    ChessMoveList *possible_moves = chess_every_possible_move(play);
    if (chess_count_moves(possible_moves) == 0) {
        chess_destroy_move_list(possible_moves);
        return 1;
    }

    chess_destroy_move_list(possible_moves);
    return 0;
}

int chess_is_draw(ChessMatch *play){
    if (chess_in_check(play, play->board.turn)) return 0;
    
    ChessMoveList *possible_moves = chess_every_possible_move(play);
    if (chess_count_moves(possible_moves) == 0) {
        chess_destroy_move_list(possible_moves);
        return 1;
    }

    chess_destroy_move_list(possible_moves);
    return 0;
}

int chess_game_end(ChessMatch *play){
    if (chess_is_draw(play) || chess_is_checkmate(play)) return 1;
    return 0;
}

int chess_game_winner(ChessMatch *play){
    if (chess_is_draw(play)) return DRAW;
    if (chess_is_checkmate(play)){
        if (play->board.turn == WHITE) return BLACK;
        if (play->board.turn == BLACK) return WHITE;
    }
    return DRAW;
}

void chess_promote_to(ChessMove *move, PiecesType promotionType){
    if (move->type == PROMOTION_NORMAL_MOVE ||
        move->type == PROMOTION_CAPTURE_MOVE){
        move->promotionType = promotionType;
    }
}

void chess_choose_and_apply_random(ChessMatch *play){
    ChessMoveList *possible_moves = chess_every_possible_move(play);
    int tam = chess_count_moves(possible_moves);
    if (tam > 0){
        int idx = rand()%tam;
        ChessMove move = chess_move_index(possible_moves, idx);
        chess_apply_move(play, &move);
    }
    chess_destroy_move_list(possible_moves);
}