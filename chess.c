#include <stdio.h>
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_primitives.h"

#include "chess_base.h"
#include "chess_error.h"

const float     width   =   640;
const float     height  =   640;

int main(int argc, char *argv[]){
    // printf("%d\n", sizeof(Piece));
    ChessGame *match = chess_new_game();

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
        return AL_CREATE_DISPLAY_ERROR;
    }

    pawn_png = al_load_bitmap("art/pawn.png");
    if (!pawn_png){
        chess_error(AL_IMG_LOAD_ERROR);
        return AL_IMG_LOAD_ERROR;
    }

    al_clear_to_color(al_map_rgb(0,0,0));

    al_identity_transform(&transform);
    al_translate_transform(&transform, (disp_data.width-640)/2, (disp_data.height-640)/2);
    al_use_transform(&transform);

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

    al_draw_line(-0.5, -0.5, -0.5, 640.5, al_map_rgb(13, 77, 77), 1.0);
    al_draw_line(-0.5, -0.5, 640.5, -0.5, al_map_rgb(13, 77, 77), 1.0);
    al_draw_line(-0.5, 640.5, 640.5, 640.5, al_map_rgb(13, 77, 77), 1.0);
    al_draw_line(640.5, -0.5, 640.5, 640.5, al_map_rgb(13, 77, 77), 1.0);

    al_identity_transform(&transform);
    al_use_transform(&transform);    

    al_flip_display();

    al_rest(7);
    al_destroy_display(display);


    chess_game_over(match);

    return 0;
}