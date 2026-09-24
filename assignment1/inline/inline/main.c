#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 30000
#define ITER 200000

int main(void)
{
    double as_re[N];
    double as_im[N];
    double bs_re[N];
    double bs_im[N];
    double cs_im[N];
    double cs_re[N];
    struct timespec bench_start_time;
    struct timespec bench_stop_time;
    double bench_diff_time;
    srand((unsigned) time(NULL));

    timespec_get(&bench_start_time, TIME_UTC);
    for (int i=0; i<ITER; i++)
    {
        for (int ix=0; ix<N; ix++)
        {
            bs_re[ix] = ix;
            bs_im[ix] = ix + 1;
            cs_re[ix] = 2 * ix;
            cs_im[ix] = 2 * ix + 1;
            as_re[ix] = (bs_re[ix]) * (cs_re[ix]) - (bs_im[ix]) * (cs_im[ix]);
            as_im[ix] = (bs_re[ix]) * (cs_im[ix]) + (bs_im[ix]) * (cs_re[ix]);
        }
    }
    timespec_get(&bench_stop_time, TIME_UTC);
    bench_diff_time = difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000.0 \
                        + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000.0;
    printf("Benchmark time for inlined loop is %fmus\n", (double)bench_diff_time / ITER);
    int random_index = rand() % N;
    printf("The random element of as is: %f+%fi\n",as_re[random_index], as_im[random_index]);
}