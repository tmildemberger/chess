#include "chess_vpiece_list.h"

ChessVPieceList* chess_create_new_vpiece_list(void){
    return ( chess_create_vpiece_list_section(INIT_SZ) );
}

ChessVPieceList* chess_create_vpiece_list_section(int size){
    ChessVPieceList *new_list = calloc(
            1, 
            sizeof (ChessVPieceList) + 
            sizeof (ChessVisualPiece*) * size );

    new_list->allocated_size = size;

    return new_list;
}

void chess_append_vpiece(ChessVPieceList *list, ChessVisualPiece *vpiece){
    ChessVPieceList *curr_section = list;
    do{
        if (curr_section->used_size < curr_section->allocated_size){
            curr_section->vpieces[curr_section->used_size] = vpiece;
            curr_section->used_size++;
            return;
        }
        if (curr_section->nxt == NULL){
            curr_section->nxt = chess_create_vpiece_list_section(
                            curr_section->allocated_size * 2 );
        }
        
        curr_section = curr_section->nxt;
    } while (1);
}

ChessVisualPiece* chess_find_vpiece_with_piece(ChessVPieceList *list, ChessPiece *piece){
    ChessVPieceList *curr_section = list;
    unsigned i;
    do {
        for (i = 0; i < curr_section->used_size; i++){
            if (curr_section->vpieces[i]->piece == piece){
                return curr_section->vpieces[i];
            }
        }
        curr_section = curr_section->nxt;
    } while (curr_section != NULL);
    return NULL;
}

void chess_destroy_vpiece_list(ChessVPieceList *vpieces){
    /**
     * passar por todas as vpeças destruindo-as
     * ir para a próxima seção da lista
     * destruir a seção anterior
     * até que não haja próxima seção
     */
    ChessVPieceList *curr_section = vpieces;
    ChessVPieceList *next = NULL;
    unsigned i;
    do {
        for (i = 0; i < curr_section->used_size; i++){
            free(curr_section->vpieces[i]);
        }
        next = curr_section->nxt;
        free(curr_section);
        curr_section = next;
    } while (curr_section != NULL);
}