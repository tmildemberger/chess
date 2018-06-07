#include <stdio.h>

#include "chess_base.h"
#include "chess_error.h"
#include "chess_render.h"
#include "chess_move_list.h"
#include "chess_piece_list.h"
#include "chess_vpiece_list.h"

const float     width   =   640;
const float     height  =   640;

const float     FPS     =   60;

const char *image_paths[] = {"art/pawn.png", "art/rook.png", "art/knight.png",
                "art/bishop.png", "art/queen.png", "art/king.png",
                "art/pawn_b.png", "art/rook_b.png", "art/knight_b.png",
                "art/bishop_b.png", "art/queen_b.png", "art/king_b.png"};
const int image_paths_sz = sizeof (image_paths) / sizeof (char*);

const int PIECE_WHITE_R = 28;
const int PIECE_WHITE_G = 72;
const int PIECE_WHITE_B = 100;
const int PIECE_BLACK_R = 85;
const int PIECE_BLACK_G = 54;
const int PIECE_BLACK_B = 2;

int main(int argc, char *argv[]){
    // printf("%d\n", sizeof(Piece));
    ChessMatch *match = chess_new_game();

    int i;
    int j;

    ChessPiece *pice = chess_piece_in_pos(match, 0, 1);
    ChessMove *mv = chess_create_move(match, pice, 0, 3);
    chess_apply_move(match, mv);
    chess_destroy_move(mv);

    pice = chess_piece_in_pos(match, 0, 6);
    mv = chess_create_move(match, pice, 0, 5);
    chess_apply_move(match, mv);
    chess_destroy_move(mv);

    pice = chess_piece_in_pos(match, 0, 3);
    mv = chess_create_move(match, pice, 0, 4);
    chess_apply_move(match, mv);
    chess_destroy_move(mv);

    pice = chess_piece_in_pos(match, 1, 6);
    mv = chess_create_move(match, pice, 1, 4);
    chess_apply_move(match, mv);
    chess_destroy_move(mv);

    pice = chess_piece_in_pos(match, 0, 0);
    mv = chess_create_move(match, pice, 0, 3);
    chess_apply_move(match, mv);
    chess_destroy_move(mv);

    int black = 0;

    for (i = match->board.board_height-1; i >= 0 ; i--){
        for (j = 0; j < match->board.board_width; j++){
            if ( (pice = chess_piece_in_pos(match, j, i)) != NULL ){
                switch (pice->type){
                    case PAWN:
                        fputc( (pice->team) == WHITE ? 'P' : 'p', stdout);
                        break;
                    case ROOK:
                        fputc( (pice->team) == WHITE ? 'R' : 'r', stdout);
                        break;
                    case KNIGHT:
                        fputc( (pice->team) == WHITE ? 'N' : 'n', stdout);
                        break;
                    case BISHOP:
                        fputc( (pice->team) == WHITE ? 'B' : 'b', stdout);
                        break;
                    case QUEEN:
                        fputc( (pice->team) == WHITE ? 'Q' : 'q', stdout);
                        break;
                    case KING:
                        fputc( (pice->team) == WHITE ? 'K' : 'k', stdout);
                        break;
                }
            } else {
                if(black){
                    fputc('+', stdout);
                } else {
                    fputc('_', stdout);
                }
            }
            black = !black;
        }
        fputc('\n', stdout);
        black = !black;
    }

    // chess_game_over(match);





    // return 0;
    ALLEGRO_DISPLAY         *display;// = NULL;
    ALLEGRO_DISPLAY_MODE    disp_data;
    ALLEGRO_TRANSFORM       transform;
    ALLEGRO_BITMAP         **images = calloc(12, sizeof (ALLEGRO_BITMAP*));
    ALLEGRO_TIMER           *timer = NULL;
    ALLEGRO_EVENT_QUEUE     *events_qu = NULL;
    ALLEGRO_BITMAP          *table = NULL;
    ALLEGRO_BITMAP          *real_board = NULL;

    if (!al_init()){
        chess_error(AL_INIT_ERROR);
        return AL_INIT_ERROR;
    }

    if (!al_init_image_addon()){
        chess_error(AL_IMAGE_INIT_ERROR);
        return AL_IMAGE_INIT_ERROR;
    }

    if (!al_init_primitives_addon()){
        chess_error(AL_PRIMITIVES_INIT_ERROR);
        return AL_PRIMITIVES_INIT_ERROR;
    }

    if (!al_install_mouse()){
        chess_error(AL_INSTALL_MOUSE_ERROR);
        return AL_INSTALL_MOUSE_ERROR;
    }

    if (!al_install_keyboard()){
        chess_error(AL_INSTALL_KEYBOARD_ERROR);
        return AL_INSTALL_KEYBOARD_ERROR;
    }


    timer = al_create_timer(1.0 / FPS);
    if (!timer){
        chess_error(AL_CREATE_TIMER_ERROR);
        return AL_CREATE_TIMER_ERROR;
    }

    /**
     * isso serve para descobrir a máxima resolução
     * que é suportada pelo display
     */
    al_get_display_mode(al_get_num_display_modes() - 1, &disp_data);

    /**
     * cria um display em fullscreen na resolução máxima
     */
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    display = al_create_display(disp_data.width, disp_data.height);
    if (!display){
        chess_error(AL_CREATE_DISPLAY_ERROR);
        al_destroy_timer(timer);
        return AL_CREATE_DISPLAY_ERROR;
    }

    for (i = 0; i < image_paths_sz; i++){
        images[i] = al_load_bitmap(image_paths[i]);
        if (!images[i]){
            chess_error(AL_IMG_LOAD_ERROR);
            al_destroy_timer(timer);
            al_destroy_display(display);
            while (--i > 0)
                al_destroy_bitmap(images[i]);
            return AL_IMG_LOAD_ERROR;
        }
    }
    // pawn_png = al_load_bitmap("art/pawn.png");
    // if (!pawn_png){
    //     chess_error(AL_IMG_LOAD_ERROR);
    //     al_destroy_timer(timer);
    //     al_destroy_display(display);
    //     return AL_IMG_LOAD_ERROR;
    // }

    table = al_create_bitmap(642, 642);
    if (!table){
        chess_error(AL_CREATE_BITMAP_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        i = image_paths_sz;
        while (--i > 0)
                al_destroy_bitmap(images[i]);
        // al_destroy_bitmap(pawn_png);
        return AL_CREATE_BITMAP_ERROR;
    }

    real_board = al_create_bitmap(disp_data.width, disp_data.height);
    if (!real_board){
        chess_error(AL_CREATE_BITMAP_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        i = image_paths_sz;
        while (--i > 0)
                al_destroy_bitmap(images[i]);
        // al_destroy_bitmap(pawn_png);
        al_destroy_bitmap(table);
        return AL_CREATE_BITMAP_ERROR;
    }

    events_qu = al_create_event_queue();
    if (!events_qu){
        chess_error(AL_CREATE_EVENT_QU_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        i = image_paths_sz;
        while (--i > 0)
                al_destroy_bitmap(images[i]);
        // al_destroy_bitmap(pawn_png);
        al_destroy_bitmap(table);
        al_destroy_bitmap(real_board);
        return AL_CREATE_EVENT_QU_ERROR;
    }

    al_register_event_source(events_qu, al_get_display_event_source(display));
    al_register_event_source(events_qu, al_get_timer_event_source(timer));
    al_register_event_source(events_qu, al_get_mouse_event_source());
    al_register_event_source(events_qu, al_get_keyboard_event_source());

    ChessVPieceList         *visual_head = NULL;

    ChessPieceList *curr = match->board.pieces;

    ChessVisualPiece *vpiece = NULL;
    visual_head = chess_create_new_vpiece_list();

    do {
        for (i = 0; i < curr->used_size; i++){
            vpiece = calloc(1, sizeof (ChessVisualPiece));
            vpiece->piece = curr->pieces[i];
            vpiece->x = vpiece->piece->pos.col*80+(disp_data.width-640)/2;
            vpiece->y = (match->board.board_height-1-vpiece->piece->pos.row)*80+(disp_data.height-640)/2;
            chess_append_vpiece(visual_head, vpiece);
        }
        curr = curr->nxt;
    } while (curr != NULL);
 
    ALLEGRO_COLOR light_square_color = al_map_rgb(6, 27, 56);//(43, 74, 111);//(13, 77, 77);
    al_set_target_bitmap(table);
    al_clear_to_color(al_map_rgb(0,0,0));

    al_identity_transform(&transform);
    al_translate_transform(&transform, 1, 1);
    al_use_transform(&transform);
    al_draw_line(-0.5, -0.5, -0.5, 640.5, light_square_color, 1.0);
    al_draw_line(-0.5, -0.5, 640.5, -0.5, light_square_color, 1.0);
    al_draw_line(-0.5, 640.5, 640.5, 640.5, light_square_color, 1.0);
    al_draw_line(640.5, -0.5, 640.5, 640.5, light_square_color, 1.0);
    // al_identity_transform(&transform);
    // al_translate_transform(&transform, 1, 1);
    // al_use_transform(&transform);

    // al_draw_line(0.5, 0.5, 0.5, 641.5, light_square_color, 1.0);
    // al_draw_line(0.5, 0.5, 641.5, 0.5, light_square_color, 1.0);
    // al_draw_line(0.5, 641.5, 641.5, 641.5, light_square_color, 1.0);
    // al_draw_line(641.5, 0.5, 641.5, 641.5, light_square_color, 1.0);
    // al_identity_transform(&transform);
    // al_translate_transform(&transform, -1, -1);
    // al_use_transform(&transform);

    // int i, 
    black = 0;
    for (i = 0; i < 64; i++){
        if (black){
            black = !black;
        } else {
            al_draw_filled_rectangle(i%8*80., i/8*80., i%8*80.+80, i/8*80.+80, light_square_color);
            // printf("x1=%.1f -- y1=%.1f\nx2=%.1f -- y2=%.1f\n", i%8*80., i/8*80., i%8*80.+80, i/8*80.+80);
            black = !black;
        }
        if (i%8==7) black = !black;

        // ChessPiece *pccc = chess_piece_in_pos(match, i%8, i/8);
        // if (pccc != NULL){
        //     al_draw_bitmap(images[pccc->name+6*pccc->team], i%8*80., i/8*80., 0);
        // }
    }

    al_set_target_bitmap(real_board);
            
    al_identity_transform(&transform);
    al_translate_transform(&transform, (disp_data.width-640)/2+1, (disp_data.height-640)/2+1);
    al_use_transform(&transform);

    al_set_target_backbuffer(display);

    int go_away = 0;
    int redraw = 0;
    int draw_pieces = 0;
    ChessVisualPiece *dragging = NULL;
    int x_diff = 0;
    int y_diff = 0;

    al_start_timer(timer);

    while (!go_away){
        ALLEGRO_EVENT event;
        al_wait_for_event(events_qu, &event);
        
        if (event.type == ALLEGRO_EVENT_TIMER)
            redraw = 1;
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            go_away = 1;
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN){
            switch (event.keyboard.keycode){
                case ALLEGRO_KEY_SPACE:
                    draw_pieces = 1;
            }
        } else if (event.type == ALLEGRO_EVENT_KEY_UP){
            switch (event.keyboard.keycode){
                case ALLEGRO_KEY_SPACE:
                    draw_pieces = 1;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    go_away = 1;
                    break;
                default:
                    break;
            }
        } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES){
            if (dragging != NULL){
                dragging->x = event.mouse.x + x_diff;
                dragging->y = event.mouse.y + y_diff;
            }
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            unsigned char r, g, b;
            if (event.mouse.x >= (disp_data.width-640)/2 && event.mouse.x <= (disp_data.width-640)/2+640
             && event.mouse.y >= (disp_data.height-640)/2 && event.mouse.y <= (disp_data.height-640)/2+640){
                al_unmap_rgb(al_get_pixel(real_board, 
                                    (event.mouse.x), 
                                    (event.mouse.y)), &r, &g, &b);

                if ((r == PIECE_BLACK_R && g == PIECE_BLACK_G && b == PIECE_BLACK_B)
                || (r == PIECE_WHITE_R && g == PIECE_WHITE_G && b == PIECE_WHITE_B)){
                    dragging = chess_find_vpiece_with_piece(visual_head,
                        chess_piece_in_pos(match,
                        (event.mouse.x - (disp_data.width-640)/2 ) / 80,
                        match->board.board_height-1-((event.mouse.y - (disp_data.height-640)/2 ) / 80)));
                    printf("im dragging this piece: %hhd %hhd %hhd %hhd\n", 
                        dragging->piece->pos.col, dragging->piece->pos.row,
                        dragging->piece->type, dragging->piece->team);
                    x_diff = dragging->x - event.mouse.x;
                    y_diff = dragging->y - event.mouse.y;
                }
             }

        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

            ChessMove *move = chess_create_move(match, dragging->piece, 
                    (event.mouse.x - (disp_data.width-640)/2 ) / 80,
                    match->board.board_height-1-((event.mouse.y - (disp_data.height-640)/2 ) / 80));
            chess_apply_move(match, move);
            // printf("i tried to move from %d %d to %d %d -- %d\n", move->fromCol,
            //     move->fromRow, move->toCol, move->toRow, move->moveType);
            printf("i tried to move from %d %d to %d %d -- %d\n", dragging->piece->pos.col,
                dragging->piece->pos.row, (event.mouse.x - (disp_data.width-640)/2 ) / 80, match->board.board_height-1-((event.mouse.y - (disp_data.height-640)/2 ) / 80), move->type);
            chess_destroy_move(move);

            dragging->x = dragging->piece->pos.col*80+(disp_data.width-640)/2;
            dragging->y = (match->board.board_height-1-dragging->piece->pos.row)*80+(disp_data.height-640)/2;

            dragging = NULL;

        }

        if (redraw && al_is_event_queue_empty(events_qu)){
            al_set_target_bitmap(real_board);

            al_clear_to_color(al_map_rgb(0,0,0));

            al_draw_bitmap(table, -1, -1, -1);

            if (draw_pieces){
                ChessVPieceList *curr = visual_head;
                while (curr != NULL){
                    for (i = 0; i < curr->used_size; i++){
                        ChessPiece *pc = curr->vpieces[i]->piece;
                        if (pc->alive){
                            al_draw_bitmap(images[pc->type + 6*pc->team],
                                            curr->vpieces[i]->x - (disp_data.width-640)/2, curr->vpieces[i]->y - (disp_data.height-640)/2, 0);
                        }
                    }
                    curr = curr->nxt;
                }
            }

            al_set_target_backbuffer(display);

            al_clear_to_color(al_map_rgb(0,0,0));

            al_draw_bitmap(real_board, 0, 0, 0);

            al_flip_display();
            redraw = 0;
        }
    }

    // al_clear_to_color(al_map_rgb(0,0,0));    

    // al_identity_transform(&transform);
    // al_translate_transform(&transform, (disp_data.width-640)/2, (disp_data.height-640)/2);
    // al_use_transform(&transform);

    // al_draw_bitmap(table, 0, 0, 0);


    // al_identity_transform(&transform);
    // al_use_transform(&transform);

    // al_flip_display();

    // al_rest(7);
    
    al_destroy_timer(timer);
    al_destroy_display(display);
    i = image_paths_sz;
    while (--i > 0)
            al_destroy_bitmap(images[i]);
    al_destroy_bitmap(table);
    al_destroy_bitmap(real_board);
    al_destroy_event_queue(events_qu);

    chess_destroy_vpiece_list(visual_head);
    


    chess_game_over(match);







    // chess_init_new_game();

    // chess_init_render_pack();

    // chess_init_timer_and_events();

    // chess_bind_viewable_objs();

    // chess_draw_base_board();

    // while (!go_away){
    //     ALLEGRO_EVENT event;
    //     al_wait_for_event(queue, &event);
    //     chess_handle_event(&event);
    // }



























    return 0;
}