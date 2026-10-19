#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef struct
{
   float r;
   float i;
} Point;

typedef struct 
{
    int val;
    char pad[56];
} int_padded;

typedef struct 
{
    const Point **v;
    Point **w;
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
    const Point **v;
    Point **w;
    int sz;
    int nthrds;
    pthread_mutex_t *mtx;
    pthread_cond_t *cnd;
    int_padded *status;
} thrd_info_check_t;

void * main_thrd(void *args)
{
    const thrd_info_t *thrd_info = (thrd_info_t*)args;
    const Point **v = thrd_info->v;
    Point **w = thrd_info->w;
    const int ib = thrd_info->ib;
    const int istep = thrd_info->istep;
    const int sz = thrd_info->sz;
    const int tx = thrd_info->tx;
    pthread_mutex_t *mtx = thrd_info->mtx;
    pthread_cond_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    for (int ix=ib; ix<sz; ix+=istep)
    {
        const Point *vix = v[ix];
        Point *wix = (Point*) malloc(sz*sizeof(Point));
        for (int jx=0; jx<sz; jx++)
        {
            wix[jx].r = vix[jx].r;
            wix[jx].i = vix[jx].i;
            // printf("The point is (%f, %f)\n", wix[jx].r, wix[jx].i);
        }
        pthread_mutex_lock(mtx);
        w[ix] = wix;
        status[tx].val = ix + istep;
        pthread_mutex_unlock(mtx);
        pthread_cond_signal(cnd);
    }
    return NULL;
}

void *main_thrd_check(void *args)
{
    const thrd_info_check_t *thrd_info = (thrd_info_check_t*) args;
    const Point **v = thrd_info->v;
    Point **w = thrd_info->w;
    const int sz = thrd_info->sz;
    const int nthrds = thrd_info->nthrds;
    pthread_mutex_t *mtx = thrd_info->mtx;
    pthread_cond_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

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
            printf("%d ", ix);
            free(w[ix]);
        }
    }
    printf("\n");
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

    // here we construct the points using randomization 
    Point **v = (Point**) malloc(num_lines*sizeof(Point*));
    Point **w = (Point**) malloc(num_lines*sizeof(Point*));
    Point *ventries = (Point*) malloc(num_lines*num_lines*sizeof(Point));
    Point point;
    srand(time(NULL));
    const float min = -2.0f;
    const float max = 2.0f;
    for (int ix=0, jx=0; ix<num_lines; ix++, jx+=num_lines)
        v[ix] = ventries + jx;
    for (int ix=0; ix<num_lines*num_lines; ix++)
    {
        ventries[ix].r = ((float)rand() / RAND_MAX) * (max - min) + min;
        ventries[ix].i = ((float)rand() / RAND_MAX) * (max - min) + min;
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
        thrds_info[tx].v = (const Point**) v;
        thrds_info[tx].w = w;
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
        thrd_info_check.v = (const Point**) v;
        thrd_info_check.w = w;
        thrd_info_check.sz = num_lines;
        thrd_info_check.nthrds = nthrds;
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
    free(w);
}