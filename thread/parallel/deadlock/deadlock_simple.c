#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

typedef struct 
{
  mtx_t *mtx;
} thrd_info_t;

int main_thrd(void *args)
{
  const thrd_info_t *thrd_info = (thrd_info_t*) args;
  mtx_lock(thrd_info->mtx);
  return 0;
}

int main()
{
  const int nthrds = 8;
  thrd_t thrds[nthrds];
  thrd_info_t thrds_info[nthrds];

  mtx_t mtx;
  mtx_init(&mtx, mtx_plain);

  for (int tx=0; tx<nthrds; tx++)
    {
      thrds_info[tx].mtx = &mtx;

      int r = thrd_create(thrds+tx, main_thrd, (void*)(thrds_info+tx));
      if (r != thrd_success)
      {
        fprint(stderr, "failed to create thread\n");
        exit(1);
      }
  }

  for (int tx=0; tx<nthrds; tx++)
  {
    int r;
    thrd_join(thrds[tx], &r);
  }
  mtx_destroy(&mtx);
  return 0;
}
