#include <stdio.h>
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_primitives.h"

const float     width   =   640;
const float     height  =   640;

int main(int argc, char *argv[]){

    ALLEGRO_DISPLAY         *display = NULL;
    ALLEGRO_DISPLAY_MODE    disp_data;
    ALLEGRO_TRANSFORM       transform;

    if(!al_init()){
        return 1;
    }

    if(!al_init_image_addon()){
        return 1;
    }

    if(!al_init_primitives_addon()){
        return 1;
    }

    al_get_display_mode(al_get_num_display_modes() - 1, &disp_data);

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    display = al_create_display(disp_data.width, disp_data.height);
    if(!display){
        return 1;
    }

    al_clear_to_color(al_map_rgb(0,0,0));

    al_identity_transform(&transform);
    al_translate_transform(&transform, (disp_data.width-640)/2, (disp_data.height-640)/2);
    al_use_transform(&transform);

    int i, black = 1;
    for(i = 0; i < 64; i++){
        if(black){
            black = !black;
        } else{
            al_draw_filled_rectangle(i%8*80., i/8*80., i%8*80.+80, i/8*80.+80, al_map_rgb(13, 77, 77));
            // printf("x1=%.1f -- y1=%.1f\nx2=%.1f -- y2=%.1f\n", i%8*80., i/8*80., i%8*80.+80, i/8*80.+80);
            black = !black;
        }
        if(i%8==7) black = !black;
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
   
    return 0;
}