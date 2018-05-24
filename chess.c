#include <stdio.h>
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_primitives.h"

#include "chess_base.h"
#include "chess_error.h"

const float     width   =   640;
const float     height  =   640;

const float     FPS     =   60;

int main(int argc, char *argv[]){
    // printf("%d\n", sizeof(Piece));
    ChessTable *match = chess_new_game();

    int i;
    int j;

    ChessPiece *pice = NULL;
    int black = 0;

    for (i = 0; i < match->table_height; i++){
        for (j = 0; j < match->table_width; j++){
            if ( (pice = chess_piece_in_pos(match, i, j)) != NULL ){
                switch (pice->name){
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
    ALLEGRO_DISPLAY         *display = NULL;
    ALLEGRO_DISPLAY_MODE    disp_data;
    ALLEGRO_TRANSFORM       transform;
    ALLEGRO_BITMAP          *pawn_png = NULL;
    ALLEGRO_TIMER           *timer = NULL;
    ALLEGRO_EVENT_QUEUE     *events_qu = NULL;
    ALLEGRO_BITMAP          *table = NULL;


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

    pawn_png = al_load_bitmap("art/pawn.png");
    if (!pawn_png){
        chess_error(AL_IMG_LOAD_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return AL_IMG_LOAD_ERROR;
    }

    table = al_create_bitmap(642, 642);
    if (!table){
        chess_error(AL_CREATE_BITMAP_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_bitmap(pawn_png);
        return AL_CREATE_BITMAP_ERROR;
    }

    events_qu = al_create_event_queue();
    if (!events_qu){
        chess_error(AL_CREATE_EVENT_QU_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_bitmap(pawn_png);
        al_destroy_bitmap(table);
        return AL_CREATE_EVENT_QU_ERROR;
    }

    al_register_event_source(events_qu, al_get_display_event_source(display));
    al_register_event_source(events_qu, al_get_timer_event_source(timer));
    al_register_event_source(events_qu, al_get_mouse_event_source());
    al_register_event_source(events_qu, al_get_keyboard_event_source());

    al_set_target_bitmap(table);

    // al_identity_transform(&transform);
    // al_translate_transform(&transform, 1, 1);
    // al_use_transform(&transform);

    al_clear_to_color(al_map_rgb(0,0,0));

    // int i, 
    black = 0;
    for (i = 0; i < 64; i++){
        if (black){
            black = !black;
        } else {
            al_draw_filled_rectangle(i%8*80., i/8*80., i%8*80.+80, i/8*80.+80, al_map_rgb(13, 77, 77));
            // printf("x1=%.1f -- y1=%.1f\nx2=%.1f -- y2=%.1f\n", i%8*80., i/8*80., i%8*80.+80, i/8*80.+80);
            black = !black;
        }
        if (i%8==7) black = !black;

        ChessPiece *pccc = chess_piece_in_pos(match, i/8, i%8);
        if (pccc != NULL){
            al_draw_bitmap(pawn_png, i%8*80., i/8*80., 0);
        }
    }

    al_set_target_backbuffer(display);

    al_clear_to_color(al_map_rgb(0,0,0));    

    al_identity_transform(&transform);
    al_translate_transform(&transform, (disp_data.width-640)/2, (disp_data.height-640)/2);
    al_use_transform(&transform);

    al_draw_bitmap(table, 0, 0, 0);

    al_draw_line(-0.5, -0.5, -0.5, 640.5, al_map_rgb(13, 77, 77), 1.0);
    al_draw_line(-0.5, -0.5, 640.5, -0.5, al_map_rgb(13, 77, 77), 1.0);
    al_draw_line(-0.5, 640.5, 640.5, 640.5, al_map_rgb(13, 77, 77), 1.0);
    al_draw_line(640.5, -0.5, 640.5, 640.5, al_map_rgb(13, 77, 77), 1.0);

    al_identity_transform(&transform);
    al_use_transform(&transform);

    al_flip_display();

    al_rest(7);
    
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_bitmap(pawn_png);
    al_destroy_bitmap(table);
    al_destroy_event_queue(events_qu);


    chess_game_over(match);

    return 0;
}