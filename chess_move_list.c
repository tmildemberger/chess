#include "chess_move_list.h"

ChessMoveList* chess_create_move_list(void){
    return ( chess_create_move_list_section(INIT_SZ) );
}

ChessMoveList* chess_create_move_list_section(int size){
    ChessMoveList *new_list = calloc(
            1, 
            sizeof (ChessMoveList) + 
            sizeof (ChessMove) * size );

    new_list->allocated_size = size;

    return new_list;
}

void chess_append_move(ChessMoveList *list, ChessMove move){
    ChessMoveList *curr_section = list;
    do{
        if (curr_section->used_size < curr_section->allocated_size){
            curr_section->moves[curr_section->used_size] = move;
            curr_section->used_size++;
            return;
        }
        if (curr_section->nxt == NULL){
            curr_section->nxt = chess_create_move_list_section(
                            curr_section->allocated_size * 2 );
        }
        
        curr_section = curr_section->nxt;
    } while (1);
}

void chess_append_move_section(ChessMoveList *list, ChessMoveList *section){
    ChessMoveList *curr_section = list;
    while (curr_section->nxt != NULL)
        curr_section = curr_section->nxt;
    curr_section->nxt = section;
}

int chess_count_moves(ChessMoveList *list){
    int count = 0;
    ChessMoveList *curr_section = list;
    while (curr_section != NULL){
        count += curr_section->used_size;
        curr_section = curr_section->nxt;
    }
    return count;
}

ChessMove chess_last_move(ChessMoveList *list){
    ChessMoveList *curr_section = list;
    if (curr_section->used_size == 0)
        return curr_section->moves[0];
    while (curr_section->nxt != NULL)
        curr_section = curr_section->nxt;
    return curr_section->moves[curr_section->used_size-1];
}

void chess_remove_last_move(ChessMoveList *list){
    ChessMoveList *curr_section = list;
    ChessMoveList *last_section = NULL;
    if (curr_section->used_size == 0)
        return;
    while (curr_section->nxt != NULL){
        last_section = curr_section;
        curr_section = curr_section->nxt;
    }
    curr_section->used_size--;
    curr_section->moves[curr_section->used_size] = (ChessMove){ (ChessSquare){ 0, 0 },
                                                                (ChessSquare){ 0, 0 },
                                                                0,
                                                                0 };
    if (curr_section->used_size == 0 &&
        last_section != NULL){
        free(curr_section);
        last_section->nxt = NULL;
    }
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