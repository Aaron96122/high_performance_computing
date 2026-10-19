#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#define MAX_ATTR_VALUES 12
#define MAX_ITERATIONS 50
#define PIXEL_TEXT_LEN 12
#define PIXEL_TEXT_SIZE (PIXEL_TEXT_LEN + 1)

static char attr_pixel_strings[MAX_ATTR_VALUES][PIXEL_TEXT_SIZE];
static char conv_pixel_strings[MAX_ITERATIONS + 1][PIXEL_TEXT_SIZE];
const unsigned int platte[MAX_ATTR_VALUES][3] =
{
    {255,   0,   0},  // 0: red
    {  0, 255,   0},  // 1: green
    {  0,   0, 255},  // 2: blue
    {255, 255,   0},  // 3: yellow
    {255,   0, 255},  // 4: magenta
    {  0, 255, 255},  // 5: cyan
    {255, 128,   0},  // 6: orange
    {128,   0, 255},  // 7: purple
    {  0, 128, 128},  // 8: teal
    {128, 128,   0},  // 9: olive
    {255, 255, 255},  // 10: white
    {  0,   0,   0}   // 11: black
};
typedef uint8_t TYPE_ATTR;
typedef uint8_t TYPE_CONV;

typedef struct 
{
    int val;
    char pad[56];
} int_padded;

typedef struct 
{
    const uint8_t **v;
    TYPE_ATTR **attractors;
    TYPE_CONV **convergences;
    int ib;
    int istep;
    int sz;
    int tx;
    pthread_mutex_t *mtx;
    pthread_cond_t *cnd;
    int_padded *status;
} thrd_info_t;

typedef struct 
{
    const uint8_t **v;
    TYPE_ATTR **attractors;
    TYPE_CONV **convergences;
    int sz;
    int nthrds;
    int exp;
    pthread_mutex_t *mtx;
    pthread_cond_t *cnd;
    int_padded *status;
} thrd_info_check_t;

void * main_thrd(void *args)
{
    const thrd_info_t *thrd_info = (thrd_info_t*)args;
    const uint8_t **v = thrd_info->v;
    TYPE_ATTR **attractors = thrd_info->attractors;
    TYPE_CONV **convergences = thrd_info->convergences;
    const int ib = thrd_info->ib;
    const int istep = thrd_info->istep;
    const int sz = thrd_info->sz;
    const int tx = thrd_info->tx;
    pthread_mutex_t *mtx = thrd_info->mtx;
    pthread_cond_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    for (int ix=ib; ix<sz; ix+=istep)
    {
        const uint8_t *vix = v[ix];
        TYPE_ATTR *attractors_line = malloc(sz*sizeof(TYPE_ATTR));
        TYPE_CONV *convergences_line = malloc(sz*sizeof(TYPE_CONV));
        for (int jx=0; jx<sz; jx++)
        {
            attractors_line[jx] = 0;
            convergences_line[jx] = 0;
            // printf("The point is (%f, %f)\n", wix[jx].r, wix[jx].i);
        }
        pthread_mutex_lock(mtx);
        attractors[ix] = attractors_line;
        convergences[ix] = convergences_line;
        status[tx].val = ix + istep;
        pthread_mutex_unlock(mtx);
        pthread_cond_signal(cnd);
    }
    return NULL;
}

void *main_thrd_check(void *args)
{
    const thrd_info_check_t *thrd_info = (thrd_info_check_t*) args;
    const uint8_t **v = thrd_info->v;
    TYPE_ATTR **attractors = thrd_info->attractors;
    TYPE_CONV **convergences = thrd_info->convergences;
    const int sz = thrd_info->sz;
    const int nthrds = thrd_info->nthrds;
    const int exp = thrd_info->exp;
    pthread_mutex_t *mtx = thrd_info->mtx;
    pthread_cond_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    // here we prepare the filenames.
    char attr_filenames[128];
    char conv_filenames[128];
    snprintf(
        attr_filenames,
        sizeof(attr_filenames),
        "newton_attractions_x%d.ppm",
        exp
    );
    snprintf(
        conv_filenames,
        sizeof(conv_filenames),
        "newton_convergences_x%d.ppm",
        exp
    );
    // here we open the output files
    FILE *attr_file = fopen(attr_filenames, "w");
    FILE *conv_file = fopen(conv_filenames, "w");
    if (attr_file == NULL || conv_file == NULL)
    {
        perror("Cannot open the output file\n");
        if (attr_file != NULL)
            fclose(attr_file);
        if (conv_file != NULL)
            fclose(conv_file);
    }
    // here we write the PPM headers
    fprintf(attr_file, "P3\n%d %d\n255\n", sz, sz);
    fprintf(conv_file, "P3\n%d %d\n255\n", sz, sz);

    for (int ix=0, ibnd; ix<sz;)
    {
        for (pthread_mutex_lock(mtx); ;)
        {
            ibnd = sz;
            for (int tx=0; tx<nthrds; tx++)
            {
                if (ibnd > status[tx].val)
                    ibnd = status[tx].val;
            }
            if (ibnd <= ix)
                pthread_cond_wait(cnd, mtx);
            else
            {
                pthread_mutex_unlock(mtx);
                break;
            }
        }
        for (; ix<ibnd; ix++)
        {
            TYPE_ATTR *attractors_line = attractors[ix];
            TYPE_CONV *convergences_line = convergences[ix];
            // here we write every pixel in the current row
            for (int cx=0; cx<sz; cx++)
            {
                TYPE_ATTR a = attractors_line[cx];
                TYPE_CONV c = convergences_line[cx];
                // here we check each pixel within the range
                assert (a < MAX_ATTR_VALUES && c <= MAX_ITERATIONS);
                // here we write each pixel into file correspondingly
                fwrite(
                    attr_pixel_strings[a],
                    1,
                    PIXEL_TEXT_LEN,
                    attr_file
                );
                fwrite(
                    conv_pixel_strings[c],
                    1,
                    PIXEL_TEXT_LEN,
                    conv_file
                );
            }
            // end each row with line breaker at the end
            fputc('\n', attr_file);
            fputc('\n', conv_file);
            free(attractors_line);
            free(convergences_line);
        }
    }
    fclose(attr_file);
    fclose(conv_file);
    return NULL;
}

int main(int argc, char *argv[])
{
    // here we check the input format and parse the input command line.
    if (argc > 4)
    {
        printf("input format shall be like ./newton -t5 -l1000 7");
    }
    int num_thread = 3;
    int num_lines = 10;
    int exponent = 2;
    for (int i=1; i<argc; i++)
    {
        if (argv[i][1] == 't')
        {
            num_thread = atoi(argv[i] + 2);
        }
        else if (argv[i][1] == 'l')
        {
            num_lines = atoi(argv[i] + 2);
        }
        else
        {
            exponent = atoi(argv[i]);
        }
    }

    // here we prepare the strings using snprintf()
    // here for the attractors:
    for (int a=0; a < MAX_ATTR_VALUES; a++)
    {
        snprintf(
            attr_pixel_strings[a],
            sizeof(attr_pixel_strings[a]),
            "%3u %3u %3u",
            platte[a][0],
            platte[a][1],
            platte[a][2]
        );
    }
    // here for the convergences
    for (unsigned int c=0; c<MAX_ITERATIONS; c++)
    {
        unsigned int gray = 255u * c / MAX_ITERATIONS;
        snprintf(
            conv_pixel_strings[c],
            sizeof(conv_pixel_strings[c]),
            "%3u %3u %3u",
            gray, 
            gray,
            gray
        );
    }

    // here we construct the points using randomization 
    uint8_t **v = (uint8_t**) malloc(num_lines*sizeof(uint8_t*));
    TYPE_ATTR **attractors = (TYPE_ATTR**) malloc(num_lines*sizeof(TYPE_ATTR*));
    TYPE_CONV **convergences = (TYPE_CONV**) malloc(num_lines*sizeof(TYPE_CONV*));
    uint8_t *ventries = (uint8_t*) malloc(num_lines*num_lines*sizeof(uint8_t));
    for (int ix=0, jx=0; ix<num_lines; ix++, jx+=num_lines)
        v[ix] = ventries + jx;
    for (int ix=0; ix<num_lines*num_lines; ix++)
    {
        ventries[ix] = 0;
        // printf("the point %i is: (%f, %f)\n", ix, ventries[ix].r, ventries[ix].i);
    }    
    
    // here we apply threads
    const int nthrds = num_thread;
    pthread_t thrds[nthrds];
    pthread_t thrd_check;
    thrd_info_t thrds_info[nthrds];
    thrd_info_check_t thrd_info_check;
    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_t cnd;
    pthread_cond_init(&cnd, NULL);
    int_padded status[nthrds];

    for (int tx=0; tx<nthrds; tx++)
    {
        thrds_info[tx].v = (const uint8_t**) v;
        thrds_info[tx].attractors = attractors;
        thrds_info[tx].convergences = convergences;
        thrds_info[tx].ib = tx;
        thrds_info[tx].istep = nthrds;
        thrds_info[tx].sz = num_lines;
        thrds_info[tx].tx = tx;
        thrds_info[tx].mtx = &mtx;
        thrds_info[tx].cnd = &cnd;
        thrds_info[tx].status = status;
        status[tx].val = 0;

        int r = pthread_create(thrds+tx, NULL, main_thrd, (void*)(thrds_info+tx));
        if (r != 0)
        {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
        // pthread_detach(thrds[tx]);
    }

    {
        thrd_info_check.v = (const uint8_t**) v;
        thrd_info_check.attractors = attractors;
        thrd_info_check.convergences = convergences;
        thrd_info_check.sz = num_lines;
        thrd_info_check.nthrds = nthrds;
        thrd_info_check.exp = exponent;
        thrd_info_check.mtx = &mtx;
        thrd_info_check.cnd = &cnd;
        thrd_info_check.status = status;

        int r = pthread_create(&thrd_check, NULL, main_thrd_check, (void*)(&thrd_info_check));
        if (r != 0)
        {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
    }

    pthread_join(thrd_check, NULL);
    for (int tx=0; tx<nthrds; tx++)
    {
        pthread_join(thrds[tx], NULL);
    }
    free(ventries);
    free(v);
    free(attractors);
    free(convergences);
}