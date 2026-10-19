#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

typedef struct {
  mtx_t *mtx[2];
} thrd_info_t;


int
main_thrd_even(
    void *args
    )
{
  const thrd_info_t *thrd_info = (thrd_info_t*) args;

  mtx_lock(thrd_info->mtx[0]);
  mtx_lock(thrd_info->mtx[1]);

  mtx_unlock(thrd_info->mtx[1]);
  mtx_unlock(thrd_info->mtx[0]);

  return 0;
}

int
main_thrd_odd(
    void *args
    )
{
  const thrd_info_t *thrd_info = (thrd_info_t*) args;

  mtx_lock(thrd_info->mtx[1]);
  mtx_lock(thrd_info->mtx[0]);

  mtx_unlock(thrd_info->mtx[0]);
  mtx_unlock(thrd_info->mtx[1]);

  return 0;
}

int
main()
{
  const int nthrds = 8;
  thrd_t thrds[nthrds];
  thrd_info_t thrds_info[nthrds];
  
  mtx_t mtx[2];
  mtx_init(mtx+0, mtx_plain);
  mtx_init(mtx+1, mtx_plain);

  for ( int tx = 0; tx < nthrds; ++tx ) {
    thrds_info[tx].mtx[0] = mtx+0;
    thrds_info[tx].mtx[1] = mtx+1;

    int r = thrd_create(
        thrds+tx,
        tx & 1 ? main_thrd_odd : main_thrd_even,
        (void*) (thrds_info+tx)
        );
    if ( r != thrd_success ) {
      fprintf(stderr, "failed to create thread\n");
      exit(1);
    }
  }

  for ( int tx = 0; tx < nthrds; ++tx ) {
    int r;
    thrd_join(thrds[tx], &r);
  }

  mtx_destroy(mtx+0);
  mtx_destroy(mtx+1);

  return 0;
}