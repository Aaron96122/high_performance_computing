#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct 
{
    int val;
    char pad[60]; // cacheline - sizeof(int)
} int_padded;

typedef struct 
{
    const float **v;
    float **w;
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
    const float **v;
    float **w;
    int sz;
    int nthrds;
    pthread_mutex_t *mtx;
    pthread_cond_t *cnd;
    int_padded *status;
} thrd_info_check_t;

int main_thrd(void *args)
{
    const thrd_info_t *thrd_info = (thrd_info_t*) args;
    const float **v = thrd_info->v;
    float **w = thrd_info->w;
    const int ib = thrd_info->ib;
    const int istep = thrd_info->istep;
    const int sz = thrd_info->sz;
    const int tx = thrd_info->tx;
    pthread_mutex_t *mtx = thrd_info->mtx;
    pthread_cond_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    // here we have ib = tx, which is the index of nthrds from 0 to 7
    // here we have istep, which is nthrds, namely, 8
    for (int ix=ib; ix<sz; ix+=istep)
    {
        // vix stores the pointer in each v[ix] where
        // ix = 0 -> *vix = v[0], namely, ventries[0]
        // ix = 8 -> *vix = v[8], namely, ventries[0+500*8]
        // ix = 16 -> *vix = v[16], namely, ventries[0+500*16]
        // ...
        // ix = 496 -> *vix = v[496], namely, ventries[0+500*496]
        // -------------------------------------
        // when ib is 1
        // ix = 1 -> *vix = v[1], namely, ventries[0+500*1]
        // ix = 9 -> *vix = v[9], namely, ventries[0+500*9]
        // ix = 17 -> *vix = v[17], namely, ventries[0+500*17]
        // ...
        // ix = 497 -> vix = v[497], namely, ventries[0+500*497]
        // **************************************
        // Therefore, we have vix stores specific index in ventries
        const float *vix = v[ix];
        // each wix is 500-float large fpr each row of vix
        float *wix = (float*) malloc(sz*sizeof(float));

        // here we have each row of vix with offset defined by v[ix]
        for (int jx=0; jx<sz; jx++)
            // here wix[0] = square root of vix[0], when vix points to v[0]
            // which points to the v[0][0], which points to ventries[0]
            // wix[1] = square root of vix[1], when vix points to v[0]
            // which points to v[0][1], which points to ventries[1]
            // ...
            // wix[499] = square root of vix[499], when vix points to v[0]
            // which points to the v[0][499], which points to ventries[499]
            // *******************
            // when ix = 8
            // then wix[0] = square root of vix[0], when vix points to v[8]
            // which points to v[8][0], which points to ventries[0+500*8+0]
            wix[jx] = sqrtf(vix[jx]);
        
        pthread_mutex_lock(mtx);
        // here the w is also a double point which points to the start of wix;
        // w[0] points to the 500 square root results of the 500 values in v[0]
        // namely, the first 500 values in ventries
        // there are around 62 ix and 8 ibs
        // so we have 62*8*500 is appriximated around 500*500
        w[ix] = wix;
        status[tx].val = ix + istep;
        pthread_mutex_unlock(mtx);
        pthread_cond_signal(cnd);
    }
    return 0;
}

int main_thrd_check(void *args)
{
    const thrd_info_check_t *thrd_info = (thrd_info_check_t*) args;
    const float **v;
    float **w = thrd_info->w;
    const sz = thrd_info->sz;
    const int nthrds = thrd_info->nthrds;
    pthread_mutex_t *mtx = thrd_info->mtx;
    pthread_cond_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;
    const float eps = 1e-1;

    for (int ix=0, ibnd; ix<sz;)
    {
        for (pthread_mutex_lock(mtx); ;)
        {
            ibnd = sz;
            for (int tx=0; tx<nthrds; tx++)
            {
                // we have different status for different threads
                // tx=0, namely, thrd_info[0].status.val = 496 + 8;
                // tx=1, namely, thrd_info[1].status.val = 497 + 8;
                // tx=2, namely, thrd_info[2].status.val = 498 + 8;
                // tx=3, namely, thrd_info[3].status.val = 499 + 8;
                // tx=4, namely, thrd_info[4].status.val = 484 + 8;
                // so we have the conditions that the ibnd could be less than or larger than sz(500);
                if (ibnd>status[tx].val)
                    ibnd = status[tx].val;
            
            if (ibnd <= ix)
                pthread_cond_wait(cnd, mtx);
            else
            {
                pthread_mutex_unlock(mtx);
                break;
            }
            }
        fprintf(stderr, "checking until %i\n", ibnd);
        for (; ix<ibnd; ix++)
        {
            int jx = sz - 1;
            float diff = v[ix][jx] -w[ix][jx] * w[ix][jx];
            if (diff < -eps || diff > eps)
            {
                fprintf(stderr, "incorrect computation at %i %i: %f %f %f\n", 
                    ix, jx, diff, v[ix][jx], w[ix][jx]);
                exit(1);
            }
            free(w[ix]);
        }
        }
    }
    return 0;
}

int main()
{
    const int sz = 500;
    float **v = (float**) malloc(sz*sizeof(float*));
    float **w = (float**) malloc(sz*sizeof(float*));
    float *ventries = (float*) malloc(sz*sz*sizeof(float));

    for (int ix=0, jx=0; ix<sz; ix++, jx+=sz)
        v[ix] = ventries + jx;
    
    for (int ix=0; ix<sz*sz; ix++)
        ventries[ix] = ix;

    const nthrds = 8;
    pthread_t thrds[nthrds];
    thrd_info_t thrds_info[nthrds];
    pthread_t thrd_check;
    thrd_info_check_t thrds_info_check[nthrds];
    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_t cnd;
    pthread_cond_init(&cnd, NULL);
    int_padded status[nthrds];

    for (int tx=0; tx<nthrds; tx++)
    {
        // v is the double point stores the index pointer to ventries
        thrds_info[tx].v = (const float**) v;
        thrds_info[tx].w = w;
        thrds_info[tx].ib = tx;
        thrds_info[tx].istep = nthrds;
        thrds_info[tx].sz = sz;
        thrds_info[tx].tx = tx;
        thrds_info[tx].mtx = &mtx;
        thrds_info[tx].cnd = &cnd;
        thrds_info[tx].status = status;
        status[tx].val = 0;
    }
}

