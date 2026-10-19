#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // here we check the input format and parse the input command line.
    if (argc > 4)
    {
        printf("Input format shall be like ./newton -t5 -l1000 7");
    }
    int num_thread = 1;
    int num_lines = 0;
    int exponent = 1;
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

    
}