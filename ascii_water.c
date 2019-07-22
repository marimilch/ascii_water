#ifndef ASCII_WATER_C
#define ASCII_WATER_C

#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>

#define clear() printf("\033[H\033[J")
#define PI 3.14159265358979323846

int get_str_len(char str[]){
    int g_len = 0;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        ++g_len;
    }
    return g_len;
}

void rotate(char str[]){
    int len = get_str_len(str);
    char first = str[0];
    for (int i = 0; i < len-1; ++i)
    {
        str[i] = str[i+1];
    }
    str[len-1] = first;
}

void print(int x, int y, char *video)
{
    char *vid_string = malloc( ((x+1)*y+1)*sizeof(double) );
    for (int i = 0; i < y; ++i)
    {

        for (int j = 0; j < x; ++j)
        {
            vid_string[i*(x+1) + j]= video[i*x + j];
        }
        vid_string[i*(x+1)+x] = '\n';
    }
    vid_string[y*x] = '\0';
    printf("%s", vid_string);
    free(vid_string);
}

void waver(int x, int y, int t, char *video, char gradient[])
{
    //get gradient length
    int g_len = get_str_len(gradient);

    //calculate vals
    double *vals = malloc( x*y*sizeof(double) );
    for (int i = 0; i < y; ++i)
    {
        for (int j = 0; j < x; ++j)
        {
            //i is row
            //j is column
            double wave = sin(t*(PI/1000))*(i+j);
            double len = sqrt(pow(wave+(i-y/2), 2) + pow(wave+(j-x/2), 2));
            vals[i*x + j] = len ;

        }
    }

    //find min and max
    double min = vals[0];
    double max = vals[0];
    for (int i = 0; i < x*y; ++i)
    {
        if (vals[i] > max){
            max = vals[i];
        }
        if (vals[i] < min){
            min = vals[i];
        }
    }

    //prepare gradient
    double block_len = (max-min)/g_len;

    //draw in video
    for (int i = 0; i < y; ++i)
    {
        for (int j = 0; j < x; ++j)
        {
            //i is row
            //j is column
            int coordinate = i*x + j;
            int g_pos = g_len;
            double val = vals[coordinate];
            double margin = 0.01 * block_len;
            while ( val > min + margin ){
                val -= block_len;
                --g_pos;
            }
            if(g_pos<0)
            {
                g_pos = 0;
            } else if(g_pos>=g_len)
            {
                g_pos = g_len-1;
            }

            video[coordinate] = gradient[g_pos];
        }
    }

    //free temps
    free(vals);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./ascii_water width height\n");
        printf("Warning: Does not look like Water, lol\n");
        return 0;
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    char *video = malloc( x*y*sizeof(char) );
    char gradient[] = " .:+xX$W";

    //printf("gradient has length: %d", get_str_len(gradient))
    //allow overflow
    int d = 1;
    for(int i = 0; i < 10; i = i + d){
        waver(x, y, i, video, gradient);
        print(x, y, video);
        printf("%d", i);
        usleep(10000);
        // rotate(gradient);
        clear();
        if(i==10 || i==1){
            d *= -1;
        }
    } 
    // draw(x, y, video, gradient);
    // print(x, y, video);
    free(video);
    return 0;
}

#endif
