#include "chess_error.h"

#include <stdio.h>

const char *err_msg_pt[] = {
                    "portuguese",
                    "erro 1",
                    "erro 2",
                    "erro 3",
                    "erro 4",
                    "erro 5",
                    "erro 6",
                    "erro 7"
                    };

const char *err_msg_en[] = {
                    "english",
                    "error 1",
                    "error 2",
                    "error 3",
                    "error 4",
                    "error 5",
                    "error 6",
                    "error 7"
                    };

const char **err_msg[] = {
                    err_msg_pt,
                    err_msg_en
                    };

void chess_error(int error_num){
    // switch (error_num){
    //     case ALLOC_ERROR:
    //         fputs(err_msg[CURR_LANG][ALLOC_ERROR], stderr);
    //         break;
    //     default:
    //         fprintf(stderr, "error finding error\n");
    //         break;
    // }
    fputs(err_msg[CURR_LANG][error_num], stderr);
}