#ifndef ASCII_WATER_C
#define ASCII_WATER_C

#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include<limits.h>

#include"video.c"

#define clear() printf("\033[H\033[J")
#define PI 3.14159265358979323846

void flat(float *z_map, size_t x, size_t y, float f){
    for (size_t i = 0; i < x*y; ++i){
        z_map[i] = f;
    }
}

void copyzmap(float *z_map_from, float *z_map_to, size_t x, size_t y){
    for (size_t i = 0; i < x*y; ++i){
        z_map_to[i] = z_map_from[i];
    }
}

void swapptrs(void **ptr1, void **ptr2){
    //swap pointers
    void *tmp_ptr = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp_ptr;
}

void wave(
    float **z_map_ptr, 
    float **z_map_before_ptr,
    float **z_map_temp_ptr,
    size_t x,
    size_t y,
    float t
){
    float *z_map = *z_map_ptr;
    float *z_map_before = *z_map_before_ptr;
    float *z_map_temp = *z_map_temp_ptr;

    float c_2 = .5f;
    float d = 1;
    float d_2 = 1;
    float my = .05f;
    float t_2 = t * t;

    float myt2 = my * t + 2;
    float c2t2d2 = c_2 * t_2 / d_2;

    float co0 = (4 - 8 * c2t2d2) / myt2;
    float co1 = (my * t - 2) / (my * t + 2);
    float co2 = (2 * c2t2d2) / myt2;

    for (int i = 1; i < y - 1; ++i){
        for (int j = 1; j < x - 1; ++j){

            float z_cross = 
                z_map[(i+1) * x + j] + 
                z_map[(i-1) * x + j] +
                z_map[i * x + j + 1] +
                z_map[i * x + j - 1] 
            ;

            z_map_temp[i * x + j] = 
                co0 * z_map[i * x + j] +
                co1 * z_map_before[i * x + j] +
                co2 * z_cross;
            ;
        }
    }

    copyzmap(z_map, z_map_before, x, y);

    swapptrs((void**) z_map_ptr, (void**) z_map_temp_ptr);
}

void tipwaterrandomly(float *z_map, size_t x, size_t y){
    //"tip" in water
    size_t randX = 1 + rand() * (x-2) / RAND_MAX;
    size_t randY = 1 + rand() * (y-2) / RAND_MAX;
    z_map[x * randY + randX] = 0.0f;
}

void tipwaterat(float *z_map, size_t px, size_t py, size_t x){
    //"tip" in water
    z_map[x * py + px] = 0.0f;
}

int min(int a, int b){
    return a < b ? a : b;
}

int max(int a, int b){
    return a > b ? a : b;
}

void tipwatertrail(
    size_t *trailx, 
    size_t *traily, 
    float *dirx,
    float *diry,
    int x,
    int y
){
    int distt = *traily - 2;
    int distl = *trailx - 2;
    int distb = y - 2 - *traily;
    int distr = x - 2 - *trailx;

    int mindist = min(min(distt, distb), min(distl, distr));
    float velocity_root = 2.0f + (float) mindist * .0625f;

    *trailx += (int) (velocity_root * (*dirx));
    *traily += (int) (velocity_root * (*diry));

    // *trailx = max(2, min(*trailx, x-2));
    // *traily = max(2, min(*traily, y-2));

    if (*trailx > x - 2){
        *trailx = x - 3;
        *dirx *= -1.0f;
    }

    else if (*trailx < 2){
        *trailx = 3;
        *dirx *= -1.0f;
    }

    if (*traily > y - 2){
        *traily = y - 3;
        *diry *= -1.0f;
    }

    else if (*traily < 2){
        *traily = 3;
        *diry *= -1.0f;
    }

    float trailrot = asin(*diry) > 0.0f ? acos(*dirx) : 2 * PI - acos(*dirx);
    // printf("%f rad\n", trailrot);
    trailrot += 0.1f;
    *dirx = cos(trailrot);
    *diry = sin(trailrot);

    // printf("%03d %03d %03d %03d\n", distt, distr, distb, distl);
    // printf("%08ddelta %08fpx/t %08frad\n", mindist, velocity_root, *trailrot);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: ./ascii_water <width> <height> <algorithm>\n");
        printf("All parameters are numerical.\n");
        printf("Algorithm 1 is random dipping, algorithm 2 is trailing.\n");
        printf("Width and height need to be at least 8 each.\n");
        return 0;
    }

    int x = max(atoi(argv[1]), 8);
    int y = max(atoi(argv[2]), 8);
    int algorithm = atoi(argv[3]);

    // char *gradient = malloc( sizeof(char) * 9 );
    char *gradient = " .:+xX$WÆ";
    char *tint = "\e[0;36m\e[44m";
    // char *gradient = "ÆÑÊŒØMÉËÈÃÂWQBÅæ#NÁþEÄÀHKRŽœXgÐêqÛŠÕÔA€ßpmãâG¶øðé8ÚÜ$ëdÙýèÓÞÖåÿÒb¥FDñáZPäšÇàhû§ÝkŸ®S9žUTe6µOyxÎ¾f4õ5ôú&aü™2ùçw©Y£0VÍL±3ÏÌóC@nöòs¢u‰½¼‡zJƒ%¤Itocîrjv1lí=ïì<>i7†[¿?×}*{+()/»«•¬|!¡÷¦¯—^ª„”“~³º²–°­¹‹›;:’‘‚’˜ˆ¸…·¨´`";

    printf("Create video\n");
    struct Video *video = createVideo(x,y,gradient,tint);

    printf("Create and init z-map current\n");
    float *z_map = malloc( sizeof(float) * x * y );
    flat(z_map, x, y, 0.5f);

    printf("Create and init z-map before\n");
    float *z_map_before = malloc( sizeof(float) * x * y );
    flat(z_map_before, x, y, 0.5f);

    printf("Create and init z-map temp\n");
    float *z_map_temp = malloc( sizeof(float) * x * y );
    flat(z_map_temp, x, y, 0.5f);

    if (video == NULL || !z_map){
        printf("Could not create video or z_map\n");
        return -1;
    }

    //printf("gradient has length: %d", get_str_len(gradient))
    //allow overflow
    int d = 1;
    float delta_t = .2f;
    printf("Start\n");

    size_t trailx = x / 2;
    size_t traily = y / 2;
    float traildirx = cos(PI/3.0f);
    float traildiry = sin(PI/3.0f);

    // z_map[y * (x/2) + (x/2)] = 0.4f;

    for(size_t i = 0; i < UINT_MAX; ++i){
        wave( &z_map, &z_map_before, &z_map_temp, x, y, delta_t);
        if( i % 10 == 0 ) {
            if (algorithm == 2){
                tipwatertrail(&trailx, &traily, &traildirx, &traildiry, x, y);
                tipwaterat(z_map, trailx, traily, x);
            } else if (algorithm == 1){
                tipwaterrandomly(z_map, x, y);
            }    
        }    
        // print_brightness_map(z_map, x, y);

        status s = render_brightness_map(video, z_map);
        if (s < 0) {
            printf("Error while rendering - Code: %d\n", s);
            return -1;
        }

        print(video);
        // printf("%d", i);
        usleep(10000);
        // rotate(gradient);
        clear();
        // if(i==10 || i==1){
        //     d *= -1;
        // }
    } 
    // draw(x, y, video, gradient);
    // print(x, y, video);
    free(video);
    free(gradient);
    return 0;
}

#endif
