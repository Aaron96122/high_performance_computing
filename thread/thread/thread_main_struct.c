#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

typedef struct 
{
  const double *v;
  int ib;
  int ie;
  double *sum;
  mtx_t *mtx;
} thrd_info_t;

int main_thrd(void *args)
{
  const thrd_info_t *thrd_info = (thrd_info_t *) args;
  const double *v = thrd_info->v;
  const int ib = thrd_info->ib;
  const int ie = thrd_info->ie;

  double sum = 0.;
  for (int ix=ib; ix<ie; ix++)
    sum += v[ix];
  
  mtx_lock(thrd_info->mtx);
  *thrd_info->sum += sum;
  mtx_unlock(thrd_info->mtx);
  return 0;
}

int main()
{
  const int sz = 1783;
  double *v = (double *) malloc(sz*sizeof(double));
  for (int ix=0; ix<sz; ix++)
    v[ix] = ix + 1;

  const int nthrds = 8;
  thrd_t thrds[nthrds];
  thrd_info_t thrds_info[nthrds];
  mtx_t mtx;
  mtx_init(&mtx, mtx_plain);

  double sum = 0.;
  const int szloc = sz / nthrds;
  for (int tx=0, ib=0; tx<nthrds; ix++, ib+=szloc)
  {
    thrds_info[tx].v = v;
    thrds_info[tx].ib = ib;
    thrds_info[tx].ie = tx != nthrds - 1 ? ib + szloc : sz;
    thrds_info[tx].sum = &sum;
    thrds_info[tx].mtx = &mtx;

    int r = thrd_create(thrds+tx, main_thrd, (void *)(thrds_info+tx));
    if (r != thrd_success)
    {
      fprintf(stderr, "failed to create thread\n");
      exit(1);
    }
  }

  for (int tx=0; tx<thrds; tx++)
  {
    int r;
    thrd_join(thrds[tx], &r);
  }

  mtx_destory(&mtx);
  return 0;
}