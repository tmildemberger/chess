#include "chess_list.h"

ChessNode* chess_list_create(void){
    ChessNode *head = calloc(1, sizeof (ChessNode));
    ChessNode *tail = calloc(1, sizeof (ChessNode));
    head->prv = tail;
    head->nxt = tail;    
    tail->prv = head;
    return head;
}

void chess_list_append_new_piece(ChessNode *head, ChessPiece *piece){
    ChessNode *new_node = calloc(1, sizeof (ChessNode));
    new_node->piece = piece;
    chess_list_append_node(head, new_node);
}

void chess_list_append_new_v_piece(ChessNode *head, ChessVisualPiece *vpiece){
    ChessNode *new_node = calloc(1, sizeof (ChessNode));
    new_node->vpiece = vpiece;
    chess_list_append_node(head, new_node);
}

void chess_list_append_new_move(ChessNode *head, ChessMove *move){
    ChessNode *new_node = calloc(1, sizeof (ChessNode));
    new_node->move = move;
    chess_list_append_node(head, new_node);
}

void chess_list_append_node(ChessNode *head, ChessNode *node){
    ChessNode *tail = head->prv;
    chess_list_insert_before(tail, node);
}

void chess_list_insert_before(ChessNode *at_right, ChessNode *to_be_inserted){
    ChessNode *at_left = at_right->prv;
    to_be_inserted->nxt = at_left->nxt;
    to_be_inserted->prv = at_left; // == at_right->prv
    at_left->nxt = to_be_inserted;
    at_right->prv = to_be_inserted;
}

void chess_list_destroy_empty(ChessNode *head){
    free(head->prv);
    free(head);
}

ChessNode* chess_list_find_piece(ChessNode *head, ChessPiece *piece){
    ChessNode *node = head->nxt;
    while (node != head->prv)
        if (node->piece == piece)
            break;
        else 
            node = node->nxt;
    return (node == head->prv ? NULL : node);
}

void chess_list_remove_node(ChessNode *node){
    if (node != NULL){
        node->prv->nxt = node->nxt;
        node->nxt->prv = node->prv;
        node->nxt = NULL;
        node->prv = NULL;
        free(node);
    }
    // } else {
    //     node->prv->nxt = NULL;
    //     node->prv = NULL;
}

ChessVisualPiece* chess_list_find_v_piece_with_piece(ChessNode *head, ChessPiece *piece){
    ChessNode *node = head->nxt;
    while (node != head->prv)
        if (node->vpiece->piece == piece)
            break;
        else 
            node = node->nxt;
    return (node == head->prv ? NULL : node->vpiece);
}

ChessNode* chess_list_find_v_piece(ChessNode *head, ChessVisualPiece *vpiece){
    ChessNode *node = head->nxt;
    while (node != head->prv)
        if (node->vpiece == vpiece)
            break;
        else 
            node = node->nxt;
    return (node == head->prv ? NULL : node);
}