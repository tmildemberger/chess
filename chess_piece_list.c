#include "chess_piece_list.h"

ChessPieceList* chess_create_new_piece_list(void){
    return ( chess_create_piece_list_section(INIT_SZ) );
}

ChessPieceList* chess_create_piece_list_section(int size){
    ChessPieceList *new_list = calloc(
            1, 
            sizeof (ChessPieceList) + 
            sizeof (ChessPiece*) * size );

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
    unsigned i;
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

ChessPiece* chess_piece_index(ChessPieceList *list, int idx){
    static int last_idx = 0;
    static ChessPieceList *last_list = NULL;
    static ChessPieceList *last_section = NULL;
    static int last_section_idx = 0;
    static ChessPiece *last_piece = NULL;
    if (idx < 0) return NULL;

    if (list == last_list){
        if (idx == last_idx){
            return last_piece;
        } else if (idx - last_idx < last_section->used_size - last_section_idx){
            last_section_idx = last_section_idx + idx - last_idx;
            last_idx = idx;
            last_piece = last_section->pieces[last_section_idx];
            return last_piece;
        } else if (last_idx - idx <= last_section_idx){
            last_section_idx = last_section_idx - last_idx + idx;
            last_idx = idx;
            last_piece = last_section->pieces[last_section_idx];
            return last_piece;
        } else if (idx < list->used_size){
            last_section =  list;
            last_section_idx = idx;
            last_idx = idx;
            last_piece = last_section->pieces[last_section_idx];
            return last_piece;
        } else if (idx > last_idx){
            ChessPieceList *curr_section = last_section;
            int curr_idx = idx - last_idx + last_section_idx;
            while (curr_idx > curr_section->used_size){
                if (curr_section->nxt == NULL){
                    return NULL;
                }
                curr_idx -= curr_section->used_size;
                curr_section = curr_section->nxt;
            }
            last_section = curr_section;
            last_section_idx = curr_idx;
            last_idx = idx;
            last_piece = last_section->pieces[last_section_idx];
            return last_piece;
        } else if (idx < last_idx){
            return NULL;
        }
    } else {
        ChessPieceList *curr_section = list;
        int curr_idx = idx;
        while (curr_idx > curr_section->used_size){
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
        last_piece = last_section->pieces[last_section_idx];
        return last_piece;
    }
    return NULL;
}

void chess_destroy_last_piece(ChessPieceList *pieces){
    ChessPieceList *last_section = NULL;
    ChessPieceList *curr_section = pieces;
    while (curr_section->nxt != NULL){
        last_section = curr_section;
        curr_section = curr_section->nxt;
    }
    curr_section->used_size--;
    free(curr_section->pieces[curr_section->used_size]);

    if (curr_section->used_size == 0 &&
        last_section != NULL){
        free(curr_section);
        last_section->nxt = NULL;
    }
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
    unsigned i;
    do {
        for (i = 0; i < curr_section->used_size; i++){
            free(curr_section->pieces[i]);
        }
        next = curr_section->nxt;
        free(curr_section);
        curr_section = next;
    } while (curr_section != NULL);
}