#pragma once

#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include<wchar.h>

#include"deps/ustring/ustring.c"

#define clear() printf("\033[H\033[J")

#define EMPTY_SPACE ' '
#define RENDER_SUCCESS 0
#define RENDER_FAIL_CHAR -1
#define RENDER_FAIL_SIZE -2

typedef short status;
typedef short tint;

struct Video
{
    size_t x;
    size_t y;
    size_t len;
    char *tint;
    char *gradient;
    char *map;
};

char *ucharat(char *str, size_t i){
    if (str == NULL) return NULL;

    while (i > 0){
        if (*str == '\0'){
            // exceeded end
            return NULL;
        }

        str += ucharlen(str);
        --i;
    }

    return str;
}

struct Video *createVideo(size_t x, size_t y, char *gradient, char *tint){
    //video
    struct Video *vid = malloc( sizeof(size_t) * 3 + sizeof(char*) * 3 );

    if (!vid) return NULL;

    size_t len = sizeof(char) * x * y * 6;

    vid->x = x;
    vid->y = y;
    vid->len = len;
    vid->gradient = gradient;
    vid->tint = tint;

    //create map + air for utf8
    char *map = malloc( len );

    vid->map = map;

    if (!map){
        free(vid);
        return NULL;
    }

    return vid;
}

void print_brightness_map(float *z_map, size_t x, size_t y){
    for (int i = 0; i < y; ++i){
        for (int j = 0; j < x; ++j){
            printf("%08f ", z_map[i * x + j]);

        }
        printf("\n");
    }
}

status render_brightness_map(struct Video *vid, float *z_map){
    //get gradient length
    size_t g_len = ustrlen(vid->gradient);

    //map length
    size_t len = vid->x * vid->y;

    //draw emptiness first (whole map)
    for (size_t i = 0; i < vid->len; ++i)
    {
        vid->map[i] = EMPTY_SPACE;        
    }

    //the further away, the darker the char
    char* map_i = vid->map;
    for (size_t i = 0; i < len; ++i)
    {

        if (map_i >= vid->map + vid->len){
            return RENDER_FAIL_SIZE;
        }

        float val = z_map[i];

        // "compress" val
        val = val < 0.0f ? 0.0f : val;
        val = val >= 1.0f ? 0.99f : val;

        // if (val < 0.0 || val >= 1.0){
        //     continue;
        // }
        
        // double division casted to integer on purpose
        // integerized brightness sets index on the gradient
        int g_pos = val * g_len;
        // printf("a%d, %da\n", g_pos, (int) g_len);

        //just in case, it is out of bounds for whatever reason (seems to happen)
        if (g_pos < 0)
        {
            g_pos = 0;
        } 
        else if (g_pos >= g_len)
        {
            g_pos = g_len-1;
        }

        char *charat = ucharat(vid->gradient, g_pos);

        if (charat == NULL){
            return RENDER_FAIL_CHAR;
        }

        size_t ulen = ucharlen(charat);


        // ustrlcpy(map_i, charat, ulen * sizeof(char) + 1);
        for (size_t j = 0; j < ulen; ++j){
            map_i[j] = charat[j];
        }
        map_i += ulen;

        // vid->map[i] = gradient[g_pos];
    }

    return RENDER_SUCCESS;
}

// void text(char *text, int x_pos, int y_pos, struct Video *v){
//     int x = v->x;
//     int y = v->y;

//     for (int i = 0; i < x && text[i] != '\0'; ++i)
//     {
//         //assume given coordinate is supposed to be center of text
//         int pos = y_pos*x + x_pos + i - get_str_len(text)/2;

//         //catch out of bounds
//         if (pos >= 0 && pos < x*y)
//             v->map[pos] = text[i];
//     }
// }

size_t strsize(char* str){
    size_t res = 0;
    if (str == NULL) return -1;

    while (*str != '\0'){
        ++res;
        ++str;
    }

    return res;
}

void print(struct Video *v){
    int x = v->x;
    int y = v->y;

    // string buffer (tint, variable utf 8 matrix, tint end)
    char *vid_string = malloc( 10 + ((x+1)*y+2)*sizeof(char) * 6 + 10 );
    char *vid_string_i = vid_string;

    if (!vid_string){
        printf("Error allocating vid_string");
        return;
    }

    char *input_vid = v->map;

    //tint
    size_t tint_length = strsize(v->tint);
    for (int i = 0; i < tint_length; ++i){
        *vid_string_i = v->tint[i];
        vid_string_i++;
    }

    //copy
    for (int i = 0; i < y; ++i){
        for (int j = 0; j < x; ++j){
            size_t ulen = ucharlen(input_vid);
            // *vid_string_i = *input_vid;

            // char *the_char = malloc( ulen * sizeof(char) + 1 );
            for (size_t k = 0; k < ulen; ++k){
                vid_string_i[k] = input_vid[k];
            }
            // the_char[ulen] = '\0';

            // printf("char: %s\n", the_char);

            vid_string_i += ulen;
            input_vid += ulen;

        }

        *vid_string_i = '\n';
        vid_string_i += 1;

    }

    //end tint
    char* end_tint = "\033[0m";
    size_t tint_end_length = strsize(end_tint);
    for (int i = 0; i < tint_end_length; ++i){
        *vid_string_i = end_tint[i];
        vid_string_i++;
    }


    //end string
    *vid_string_i = '\0';
    ++vid_string_i;

    printf("%s\n", vid_string);
    free(vid_string);
}
