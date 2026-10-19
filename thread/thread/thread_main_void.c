#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

int main_thrd(void *args)
{
  if (args != NULL)
      return 1;
  return 0;
}

int main()
{
  const int nthrds = 8;
  thrd_t thrds[nthrds];

  for (int tx=0; tx<nthrds; tx++)
  {
    int r=0;
    if (tx & 1)
      r = thrd_create(thrds+tx, main_thrd, (void*)main);
    else
      r = thrd_create(thrds+tx, main_thrd, NULL);

    if (r != thrd_success)
    {
      fprintf(stderr, "failed to create thread\n");
      exit(1);
    }
  }

  for (int ix=0; ix<thrds; ix++)
  {
    int r;
    thread_join(thrds[ix], &r);
    if (ix & 1)
      assert (r == 1);
    else
      assert (r == 0);
  }
  return 0;
}