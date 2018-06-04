#include "chess_move_list.h"

ChessMoveList* chess_create_move_list(void){
    ChessMoveList *list = calloc(1, 
            sizeof (ChessMoveList) + sizeof (ChessMove[INIT_SZ]));

    list->allocated_size = INIT_SZ;

    return list;
}