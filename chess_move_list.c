#include "chess_move_list.h"

ChessMoveList* chess_create_move_list(void){
    ChessMoveList *list = calloc(1, 
            sizeof (ChessMoveList) + sizeof (ChessMove[INIT_SZ]));

    list->allocated_size = INIT_SZ;

    return list;
}

void chess_destroy_move_list(ChessMoveList* moves){
    ChessMoveList *curr_section = moves;
    ChessMoveList *next = NULL;
    do {
        next = curr_section->nxt;
        free(curr_section);
        curr_section = next;
    } while (curr_section != NULL);
}