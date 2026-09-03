#include <stdio.h>

void printf_from_snd_argument(int, char*[]);
int main(
    int argc,
    char *argv[]
    )
{
  printf("command line arguments\n");
  
  for ( int ix = 0; ix < argc; ++ix ) {
    printf("%s\n", argv[ix]);
  }

  return 0;
}

void printf_from_snd_argument(int argc, char *argv[])
{
    for (int ix=3; ix<argc; ix++)
    {
        printf("%s\n", argv[ix]);
    }
}
