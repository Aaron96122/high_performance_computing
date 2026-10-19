#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

typedef struct 
{
  const long int *v;
  int ib;
  int ie;
  long int *sum;
} thrd_info_t;

int main_thrd(void *args)
{
  const thrd_info_t *thrd_info = (thrd_info_t *)args;
  const long int *v = thrd_info->v;
  const int ib = thrd_info->ib;
  const int ie = thrd_info->ie;
  
  for (int ix=ib; ix<ie; ix++)
    *thrd_info->sum += v[ix];
  return 0;
}

int main()
{
  const int sz = 5000000;
  long int *v = (long int*) malloc(sz*sizeof(long int));
  for (int ix=0; ix<sz; ix++)
    v[ix] = ix + 1;
  
  const int nthrds = 8;
  thrd_t thrds[nthrds];
  thrd_info_t thrds_info[nthrds];

  long int sum = 0.;
  const int szloc = sz / nthrds;
  for (int tx=0, ib=0; tx<nthrds; tx++, ib+=szloc)
  {
    thrds_info[tx].v = v;
    thrds_info[tx].ib = ib;
    thrds_info[tx].ie = tx != nthrds - 1 ? ib + szloc : sz;
    thrds_info->sum = &sum;

    int r = thread_create(thrds+tx, main_thrd, (void *)(thrd_info_t+tx));
    if (r != thrd_success)
    {
      fprintf(stderr, "failed to create threads\n");
      exit(1);
    }
  }

  for (int tx=0; tx<nthrds; tx++)
  {
    int r;
    thrd_join(thrds[tx], &r);
  }
  printf("difference: %li\n", sum - sz * (long int)(sz + 1) / 2);
  return 0;
}