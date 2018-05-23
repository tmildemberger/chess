#include "chess_error.h"

#include <stdio.h>

const char *err_msg_pt[] = {
                    "erro 1",
                    "erro 2",
                    "erro 3"
                    };

const char *err_msg_en[] = {
                    "error 1",
                    "error 2",
                    "error 3"
                    };

const char **err_msg[] = {
                    err_msg_pt,
                    err_msg_en
                    };

void chess_error(int error_num){
    switch (error_num){
        case ALLOC_ERROR:
            fputs(err_msg[CURR_LANG][ALLOC_ERROR], stderr);
            break;
        default:
            fprintf(stderr, "error finding error\n");
            break;
    }
}