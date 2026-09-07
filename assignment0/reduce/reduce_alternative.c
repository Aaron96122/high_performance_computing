#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int size = 10;

    /* Here we define an array of size 10 where each element contains a pointer (8 bytes)*/
    int **as = malloc(sizeof(int*) * size);
    if (as == NULL)
    {
        return 1;
    }
    
    for (size_t ix=0; ix<size; ix++)
    {
        /* Here, each as[ix] is a pointer, pointing to another array of size 10 whose value is integers.*/
        as[ix] = malloc(sizeof(int) * size);
        if (as[ix] == NULL)
        {
            return 2;
        }
    }

    for (size_t ix=0; ix<size; ix++)
    {
        for (size_t jx=0;jx<size; jx++)
        {
            as[ix][jx] = 0;
        }
    }
    printf("%d\n", as[0][0]);

    for (size_t ix=0; ix<size; ix++)
    {
        free(as[ix]);
    }
    free(as);
}