#include "chess_base.h"
#include "chess_piece_list.h"
#include "chess_move_list.h"

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

int (*chess_apply_moves[])(ChessMatch*, ChessPiece*, ChessMove) = {
    chess_apply_invalid_move,
    chess_apply_normal_move,
    chess_apply_capture_move,
    chess_apply_castling_move,
    chess_apply_en_passant_move,
    chess_apply_promotion_move
};

int (*chess_pieces_ranges[])(ChessMatch*, ChessSquare, ChessSquare, CheckType) = {
    chess_empty_squares,
    chess_empty_squares,
    chess_ignore_squares,
    chess_empty_squares,
    chess_empty_squares,
    chess_empty_squares
};

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
ChessPiece* chess_piece_in_pos(ChessMatch *play, ChessSquare square){
    return chess_find_piece_in_square(play->board.pieces, 
                                      square);
}

/**
 * cria uma jogada com tudo dentro
 */
ChessMove chess_create_move(ChessMatch *play, 
                            ChessPiece *piece,
                            ChessSquare toSquare,
                            PiecesType targetType){
    /**
     * @todo: função pra ver se a peça pode se mover
     * (por enquanto só retorna sim), função pra ver se 
     * o movimento pode ser normal (se sim e se for um peão,
     * ver se ele chegou à última casa), funções pra ver se
     * o movimento pode ser especial
     */
    ChessPiece *target = chess_piece_in_pos(play, toSquare);
    ChessMove move;
    if (piece == NULL ||
        chess_piece_cant_move(play, piece) ||
        chess_square_outside_board(play, toSquare) ||
        chess_same_square(piece->pos, toSquare) ||
        chess_not_in_your_turn(play, piece) ||
        chess_friendly_fire(play, piece, target) ){
        move.type = INVALID_MOVE;
        return move;
    }
    move.from = piece->pos;
    move.to = toSquare;
    move.targetType = targetType;
    move.type = chess_analize_move(play, piece, move);
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

    if (otherPawn != NULL &&
        otherPawn->type == PAWN &&
        otherPawn->movs == 1 &&
        (lastMove.to.row - lastMove.from.row == -2 ||
        lastMove.to.row - lastMove.from.row == 2) &&
        chess_same_square(otherPawn->pos, 
                          (ChessSquare){ move.to.col, piece->pos.row }))
        return EN_PASSANT_MOVE;
    else 
        return INVALID_MOVE;
}

MoveType chess_castling_move(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove move){
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
    if (move.to.row == lastRow && chess_pawn_ok_normal(piece, dx, dy))
        return PROMOTION_MOVE;
    else 
        return INVALID_MOVE;
}

MoveType chess_pawn_specials(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove move){
    MoveType type = chess_en_passant(play, piece, move);
    if (type != INVALID_MOVE) return type;
    return chess_promotion_move(play, piece, move);
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
        return chess_pieces_ranges[piece->type](play, move.from, move.to, NO_LAST) &&
               chess_capture_moves[piece->type](piece, dx, dy) ? 
               CAPTURE_MOVE : 
               INVALID_MOVE;
    } else {
        return chess_pieces_ranges[piece->type](play, move.from, move.to, LAST) &&
               chess_normal_moves[piece->type](piece, dx, dy) ?
               NORMAL_MOVE :
               INVALID_MOVE;
    }
}

/**
 * no tabuleiro enviado, realiza uma jogada que se
 * presume ser válida
 */
void chess_apply_move(ChessMatch *play, ChessMove move){
    ChessPiece *piece = chess_piece_in_pos(play, move.from);
    if (chess_apply_moves[move.type](play, piece, move)){
        play->board.movements++;
        play->board.turn++;
        play->board.turn %= 2;
        chess_append_move(play->record, move);
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
    return (chess_piece_in_pos(play, square) == NULL);
}

int chess_safe_square(ChessMatch* play, ChessSquare square){
    return 1;
}

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
    dx = dx>0 ? dx : -dx;
    dy = dy>0 ? dy : -dy;
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
    return dx <= 1 && dy <= 1;
}

int chess_apply_invalid_move(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove move){
    return 0;
}

int chess_apply_normal_move(ChessMatch *play, 
                            ChessPiece *piece, 
                            ChessMove move){
    chess_put_piece_in(piece, move.to);
    return 1;
}

int chess_apply_capture_move(ChessMatch *play, 
                             ChessPiece *piece, 
                             ChessMove move){
    (chess_piece_in_pos(play, move.to))->alive = 0;
    chess_put_piece_in(piece, move.to);
    return 1;
}

int chess_apply_castling_move(ChessMatch *play, 
                              ChessPiece *piece, 
                              ChessMove move){
    unsigned short lastCol = ( play->board.board_height ) * 
                             ( 2 + move.to.col - move.to.row) / 2;
    chess_put_piece_in(
        chess_piece_in_pos(play, (ChessSquare){ lastCol, move.to.row }),
        (ChessSquare){ lastCol ? move.to.col-1 : move.to.row+1, move.to.row });
    chess_put_piece_in(piece, move.to);
    return 1;
}

int chess_apply_en_passant_move(ChessMatch *play, 
                                ChessPiece *piece, 
                                ChessMove move){
    ChessSquare unlucky = { move.to.col, move.from.row };
    (chess_piece_in_pos(play, unlucky))->alive = 0;
    chess_put_piece_in(piece, move.to);
    return 1;
}

int chess_apply_promotion_move(ChessMatch *play, 
                               ChessPiece *piece, 
                               ChessMove move){
    chess_put_piece_in(piece, move.to);
    piece->type = move.targetType;
    return 1;
}

void chess_put_piece_in(ChessPiece *piece, ChessSquare toSquare){
    piece->movs++;
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

int chess_ignore_squares(ChessMatch *play, 
                         ChessSquare fromSquare,
                         ChessSquare toSquare,
                         CheckType check){
    return 1;
}