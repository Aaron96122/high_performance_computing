#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <omp.h>

// here we include some necessary prototypes
static int16_t parse_coord(const char *s);
const int MAX_POINTS = 100000;
const int BLOCK_SIZE = 256;

// here we define the type of point in int16_manner
typedef struct 
{
    int16_t x;
    int16_t y;
    int16_t z;
} Point;

int main(void)
{
    FILE *fp = fopen("./cells.dat", "r");
    if (fp == NULL)
    {
        perror("Could not open the data file cells.dat");
        return EXIT_FAILURE;
    }
    // here we allocate the memory to store all the points we have
    Point *points = malloc(MAX_POINTS * sizeof(Point));
    if (points == NULL)
    {
        return -1;
    }
    // here we make an array for each line due to the fixed format
    // we have 24 bytes in total with terminator at the end of each line
    // also, we have some extra margins
    char line[32];
    Point p;
    int index =0;
    while (fgets(line, sizeof(line), fp))
    {
        p.x = parse_coord(line);
        p.y = parse_coord(line + 8);
        p.z = parse_coord(line + 16);  
        points[index] = p;
        // printf("coordinates are: (%i, %i, %i)\n", points[index].x, points[index].y, points[index].z);
        index += 1;
    }
    fclose(fp);
    // here we define our array of distance counts
    // we use the unsigned int64 here since all the distance are non-negative and might be exceeding 32-bit range.
    // the maximum distance between two points is: sqrt(20^2 + 20^2 + 20^2)
    uint64_t counts[3465] = {0};
    
    // // the naive bechmark
    // for (int i=0; i<index; i++)
    // {
    //     for (int j=i + 1; j<index; j++)
    //     {
    //         dx = (int32_t)points[i].x - points[j].x;
    //         dy = (int32_t)points[i].y - points[j].y;
    //         dz = (int32_t)points[i].z - points[j].z;
    //         dist_sq = dx * dx + dy * dy + dz * dz;
    //         distance_index = (int)lroundf(sqrtf((float)dist_sq) / 10.0f);
    //         counts[distance_index]++;
    //     }    
    // }

    // cache blocking for distance computing with the BLOCK_SIZE of 10;
    #pragma omp parallel
    {
        // every thread gets its own histogram
        uint64_t local_counts[3465] = {0};

        #pragma omp for schedule(dynamic, 1)
        for (int bi=0; bi<index; bi+=BLOCK_SIZE)
        {
            int i_end = bi + BLOCK_SIZE;
            if (i_end > index)
            {
                i_end = index;
            }
            for (int bj=bi; bj<index; bj+=BLOCK_SIZE)
            {
                int j_end = bj + BLOCK_SIZE;
                if (j_end > index)
                {
                    j_end = index;
                }
                for (int i=bi; i<i_end; i++)
                {
                    int j_start;
                    if (bi == bj)
                    {
                        j_start= i+1;
                    }
                    else
                    {
                        j_start = bj;
                    }
                    for (int j=j_start; j<j_end; j++)
                    {
                        int32_t dx = (int32_t)points[i].x - points[j].x;
                        int32_t dy = (int32_t)points[i].y - points[j].y;
                        int32_t dz = (int32_t)points[i].z - points[j].z;
                        int32_t dist_sq = dx * dx + dy * dy + dz * dz;
                        int distance_index = (int)lroundf(sqrtf((float)dist_sq) / 10.0f);
                        local_counts[distance_index]++;
                    }
                }
            }
        }
        // merge all the histograms of all threads here
        #pragma omp critical
        {
            for (int k=0; k<3465; k++)
            {
                counts[k] += local_counts[k];
            }
        }
    }
    // here we free the memory of points here
    free(points);
    for (int k=0; k<3465; k++)
    {
        if (counts[k] && counts[k] > 0)
        {
            printf("%02d.%02d is %llu\n", 
                k / 100,
                k % 100,
                counts[k]);
        }
    }
}

static int16_t parse_coord(const char *s)
{
    int value = 
        (s[1] - '0') * 10000 +
        (s[2] - '0') * 1000 +
        (s[4] - '0') * 100 + 
        (s[5] - '0') * 10 +
        (s[6] - '0');

    if (s[0] == '-')
    {
        value = -value;
    }
    return (int16_t) value;
}