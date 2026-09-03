#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Arrays (when declared inside of functions) are 
    // allocated on the "stack", a particularly faster spot in memory.
    {
        int a[10];
        for (int ix=0; ix<10; ix++)
        {
            a[ix] = 15 + 2 * ix;
        }
        for (int ix=0; ix < 9; ix++)
        {
            printf("%i ", a[ix]);
        }
    }
    // Memory can be allocated through calls to malloc and related functions.
    // Here both allocation and freeling are explicit. 
    // This type pf memory allocation happens on the "heap", which is slower 
    // than the stack.
    {
        int *a = (int*) malloc(10*sizeof(int));
        if ( a == NULL )
        {
            return -1;
        }

        for (int ix=0; ix<9; ix++)
        {
            printf("%i ", a[ix]);
        }
        printf("%i ", a[9]);

        free(a);
    }
    return 0;
}
