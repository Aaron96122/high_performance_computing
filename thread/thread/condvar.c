#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

typedef struct
{
    const long int *v;
    int ib;
    int ie;
    long int *sum;
    mtx_t *mtx;
    cnd_t *cnd;
} thrd_info_main_t;

typedef struct
{
    long int *sum;
    long int res;
    mtx_t *mtx;
    cnd_t *cnd;
} thrd_info_succ_t;

int main_thrd(void *args)
{
    const thrd_info_main_t *thrd_info = (thrd_info_main_t *) args;
    const long int *v = thrd_info->v;
    const int ib = thrd_info->ib;
    const int ie = thrd_info->ie;

    for (int ix=ib; ix<ie; ix++)
    {
        mtx_lock(thrd_info->mtx);
        *thrd_info->sum += v[ix];
        mtx_unlock(thrd_info->mtx);
    }
    cnd_signal(thrd_info->cnd);
    return 0;
}

int main_thrd_succ(void *args)
{
    const thrd_info_succ_t *thrd_info = (thrd_info_succ_t *) args;
    const long int res = thrd_info->res;
    long int *sum = thrd_info->sum;

    mtx_lock(thrd_info->mtx);
    while (*sum != res)
        cnd_wait(thrd_info->cnd, thrd_info->mtx);
    mtx_unlock(thrd_info->mtx);
    printf("Successfully completed\n");
    fflush(stdout);
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
    thrd_info_main_t thrds_info[nthrds];

    thrd_t thrd_succ;
    thrd_info_succ_t thrds_info_succ[nthrds];

    mtx_t mtx;
    mtx_init(&mtx, mtx_plain);
    cnd_t cnd;
    cnd_init(&cnd);

    long int sum = 0;
    const long int res = (long int) sz * (sz + 1) / 2;
    const int szloc = sz / nthrds;
    for (int tx=0, ib=0; tx<nthrds; tx++, ib+=szloc)
    {
        thrds_info[tx].v = v;
        thrds_info[tx].ib = ib;
        thrds_info[tx].ie = tx != nthrds - 1 ? ib + szloc : sz;
        thrds_info[tx].sum = &sum;
        thrds_info[tx].mtx = &mtx;
        thrds_info[tx].cnd = &cnd;

        int r = thread_create(thrds+tx, main_thrd, (void*)(thrds_info+tx));
        if (r != thrd_success)
        {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
    }

    {
        thrds_info_succ.sum = &sum;
        thrds_info_succ.res = res;
        thrds_info_succ.mtx = &mtx;
        thrds_info_succ.cnd = &cnd;

        int r = thread_create(&thrd_succ, main_thrd_succ, (void*)(&thrds_info_succ));
        if (r != thrd_success)
        {
            fprintf(stderr, "failed to create threads\n");
            exit(1);
        }
    }

    {int r;
    thrd_join(thrd_succ, &r);}

    for (int tx=0; tx<nthrds; tx++)
    {
        int r;
        thrd_join(thrd[ix], &r);
    }
    mtx_destroy(&mtx);
    cnd_destroy(&cnd);
    return 0;
}





typedef struct {
  const long int *v;
  int ib;
  int ie;
  long int *sum;
  mtx_t *mtx;
  cnd_t *cnd;
} thrd_info_main_t;

typedef struct {
  long int *sum;
  long int res;
  mtx_t *mtx;
  cnd_t *cnd;
} thrd_info_succ_t;

int
main_thrd(
    void *args
    )
{
  const thrd_info_main_t *thrd_info = (thrd_info_main_t*) args;
  const long int* v = thrd_info->v;
  const int ib = thrd_info->ib;
  const int ie = thrd_info->ie;

  for ( int ix = ib; ix < ie; ++ix ) {
    mtx_lock(thrd_info->mtx);
    *thrd_info->sum += v[ix];
    mtx_unlock(thrd_info->mtx);
  }

  // This signals to one waiting call; cnd_broadcast signals to all.
  cnd_signal(thrd_info->cnd);

  return 0;
}

int
main_thrd_succ(
    void *args
    )
{
  const thrd_info_succ_t *thrd_info = (thrd_info_succ_t*) args;
  const long int res = thrd_info->res;
  long int *sum = thrd_info->sum;

  mtx_lock(thrd_info->mtx);
  while ( *sum != res )
    // The mutex when invoking cnd_wait must be helt. After completion of the
    // function call it will be helt again.
    cnd_wait(thrd_info->cnd, thrd_info->mtx);
  mtx_unlock(thrd_info->mtx);

  // Usually when printing from threads, we need to synchronize to ensure that
  // no print happens in any other thread. Here we know that we are the only
  // thread.
  printf("Successfully completed\n");
  // We include fflush for the purpose of illustration. When in a truely
  // concurrent situation, we should flush the buffer before finishing the
  // synchronized block (e.g. unlock the mutex that we locked before printing).
  fflush(stdout);

  return 0;
}

int
main()
{
  const int sz = 5000000;
  long int *v = (long int*) malloc(sz*sizeof(long int));

  for ( int ix = 0; ix < sz; ++ix )
    v[ix] = ix+1;


  const int nthrds = 8;
  thrd_t thrds[nthrds];
  thrd_info_main_t thrds_info[nthrds];
  
  thrd_t thrd_succ;
  thrd_info_succ_t thrd_info_succ;

  mtx_t mtx;
  mtx_init(&mtx, mtx_plain);

  cnd_t cnd;
  cnd_init(&cnd);

  long int sum = 0;
  const long int res = (long int) sz * (sz + 1) / 2;

  const int szloc = sz / nthrds;
  for ( int tx = 0, ib = 0; tx < nthrds; ++tx, ib += szloc ) {
    thrds_info[tx].v = v;
    thrds_info[tx].ib = ib;
    thrds_info[tx].ie = tx != nthrds - 1 ? ib + szloc : sz;
    thrds_info[tx].sum = &sum;
    thrds_info[tx].mtx = &mtx;
    thrds_info[tx].cnd = &cnd;

    int r = thrd_create(thrds+tx, main_thrd, (void*) (thrds_info+tx));
    if ( r != thrd_success ) {
      fprintf(stderr, "failed to create thread\n");
      exit(1);
    }
  }

  {
    thrd_info_succ.sum = &sum;
    thrd_info_succ.res = res;
    thrd_info_succ.mtx = &mtx;
    thrd_info_succ.cnd = &cnd;

    int r = thrd_create(&thrd_succ, main_thrd_succ, (void*) (&thrd_info_succ));
    if ( r != thrd_success ) {
      fprintf(stderr, "failed to create thread\n");
      exit(1);
    }
  }

  // Only for the purpose of illustration, we join the thread that was created
  // last first. The appropriate order depends on the program logic. It is not
  // determined by the C standard.
  {
    int r;
    thrd_join(thrd_succ, &r);
  }

  for ( int tx = 0; tx < nthrds; ++tx ) {
    int r;
    thrd_join(thrds[tx], &r);
  }

  mtx_destroy(&mtx);
  cnd_destroy(&cnd);

  return 0;
}