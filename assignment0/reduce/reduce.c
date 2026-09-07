#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int size = 10;

    /* Here we define the pointer ascentries 
        which points to a 10 * 10 array where each value is int (4 bytes).
        Thus, this piece of code makes a memory of 4 * 10 * 10 for 100 integers
        whose first address is stored in the pointer ascentries.*/
    int *ascentries = malloc(sizeof(int) * size * size); 
    if (ascentries == NULL)
    {
        return 1;
    }
    /* Here we define another pointer as
        which points to a 10 size array where each value is also a pointer int* (8 bytes).
        Thus, this piece of code makes a memory of 8 * 10 = 80 bytes for 10 pointers whose
        first address is stored in the pointer as.*/
    int **as = malloc(sizeof(int*) * (size)); 
    if (as == NULL)
    {
        return 2;
    }

    for (size_t ix=0, jx=0; ix < size; ix++, jx+=size)
    {
        /* Here we change the adress value in each element of as array
            e.g. when ix=0, jx=0, as[0] = the address of the first value in ascentries
            namely, as[0] = &ascentries[0]
            when ix=1, jx=10, as[1] = the address of original adress stored in ascentries moveing forward by 10
            namely, as[1] = &ascentries[10]
            Likewise:
            as[2] = &ascentries[20]...*/
        as[ix] = ascentries + jx; 
    }
    // printf("%d\n", as[0][0]);

    for (size_t ix=0; ix<size; ix++)
    {
        for (size_t jx=0; jx<size; jx++)
        {
            /* I have no idea what's the as[ix][jx] here.*/
            as[ix][jx] = 0;
        }
    }
    printf("%d\n", as[0][0]);
    free(as);
    free(ascentries);
}