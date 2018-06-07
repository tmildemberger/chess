#include "chess_piece_list.h"

ChessPieceList* chess_create_new_piece_list(void){
    return ( chess_create_piece_list_section(INIT_SZ) );
}

ChessPieceList* chess_create_piece_list_section(int size){
    ChessPieceList *new_list = calloc(
            1, 
            sizeof (ChessPieceList) + 
            sizeof (ChessPiece) * size );

    new_list->allocated_size = size;

    return new_list;
}

void chess_append_piece(ChessPieceList *list, ChessPiece *piece){
    ChessPieceList *curr_section = list;
    do{
        if (curr_section->used_size < curr_section->allocated_size){
            curr_section->pieces[curr_section->used_size] = piece;
            curr_section->used_size++;
            return;
        }
        if (curr_section->nxt == NULL){
            curr_section->nxt = chess_create_piece_list_section(
                            curr_section->allocated_size * 2 );
        }
        
        curr_section = curr_section->nxt;
    } while (1);
}

ChessPiece* chess_find_piece_in_square(ChessPieceList *list, ChessSquare square){
    ChessPieceList *curr_section = list;
    int i;
    do {
        for (i = 0; i < curr_section->used_size; i++){
            if (curr_section->pieces[i]->alive &&
                curr_section->pieces[i]->pos.col == square.col &&
                curr_section->pieces[i]->pos.row == square.row ){
                return curr_section->pieces[i];
            }
        }
        curr_section = curr_section->nxt;
    } while (curr_section != NULL);
    return NULL;
}

void chess_destroy_piece_list(ChessPieceList *pieces){
    /**
     * passar por todas as peças destruindo-as
     * ir para a próxima seção da lista
     * destruir a seção anterior
     * até que não haja próxima seção
     */
    ChessPieceList *curr_section = pieces;
    ChessPieceList *next = NULL;
    int i;
    do {
        for (i = 0; i < curr_section->used_size; i++){
            free(curr_section->pieces[i]);
        }
        next = curr_section->nxt;
        free(curr_section);
        curr_section = next;
    } while (curr_section != NULL);
}