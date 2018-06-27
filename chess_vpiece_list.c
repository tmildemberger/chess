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

ChessVisualPiece* chess_v_piece_index(ChessVPieceList *list, unsigned int idx){
    static unsigned int last_idx = 0;
    static ChessVPieceList *last_list = NULL;
    static ChessVPieceList *last_section = NULL;
    static unsigned int last_section_idx = 0;
    static ChessVisualPiece *last_v_piece = NULL;
    // if (idx < 0) return NULL;

    if (list == last_list){
        if (idx == last_idx){
            return last_v_piece;
        } else if (idx - last_idx < last_section->used_size - last_section_idx){
            last_section_idx = last_section_idx + idx - last_idx;
            last_idx = idx;
            last_v_piece = last_section->vpieces[last_section_idx];
            return last_v_piece;
        } else if (last_idx - idx <= last_section_idx){
            last_section_idx = last_section_idx - last_idx + idx;
            last_idx = idx;
            last_v_piece = last_section->vpieces[last_section_idx];
            return last_v_piece;
        } else if (idx < list->used_size){
            last_section =  list;
            last_section_idx = idx;
            last_idx = idx;
            last_v_piece = last_section->vpieces[last_section_idx];
            return last_v_piece;
        } else if (idx > last_idx){
            ChessVPieceList *curr_section = last_section;
            unsigned int curr_idx = idx - last_idx + last_section_idx;
            while (curr_idx >= curr_section->used_size){
                if (curr_section->nxt == NULL){
                    return NULL;
                }
                curr_idx -= curr_section->used_size;
                curr_section = curr_section->nxt;
            }
            last_section = curr_section;
            last_section_idx = curr_idx;
            last_idx = idx;
            last_v_piece = last_section->vpieces[last_section_idx];
            return last_v_piece;
        } else if (idx < last_idx){
            return NULL;
        }
    } else {
        ChessVPieceList *curr_section = list;
        unsigned int curr_idx = idx;
        while (curr_idx >= curr_section->used_size){
            if (curr_section->nxt == NULL){
                return NULL;
            }
            curr_idx -= curr_section->used_size;
            curr_section = curr_section->nxt;
        }
        last_list = list;
        last_section = curr_section;
        last_section_idx = curr_idx;
        last_idx = idx;
        last_v_piece = last_section->vpieces[last_section_idx];
        return last_v_piece;
    }
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