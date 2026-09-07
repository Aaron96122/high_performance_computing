#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int size=10;

    // here we adopted the memory allocation without voiding memory allocation. 
    /* here we initialize the matrix */
    int **as = malloc(sizeof(int*) * size);
    if (as == NULL)
    {
        return 1;
    }
    for (size_t ix=0; ix<size; ix++)
    {
        as[ix] = malloc(sizeof(int) * size);
        if (as[ix] == NULL)
        {
            return 1;
        }
    }
    
    for (size_t ix=0; ix<size; ix++)
    {
        for (size_t jx=0; jx<size; jx++)
        {
            as[ix][jx] = ix * jx;
            // printf("%d ", as[ix][jx]);
        }
        // printf("\n");
    }

    // here we write this matrix into the file out_fragment
    FILE *f = fopen("./out_fragment", "wb");
    if (f == NULL)
    {
        return 2;
    }
    /* alternative 1 : not work*/
    // fwrite(as, sizeof(int), size*size, f);

    /* alternative 2 : work*/
    // for (size_t ix=0; ix<size; ix++)
    // {
    //     fwrite(as[ix], sizeof(int), size, f);
    // }

    /* alternative 3: not work*/
    fwrite(as[0], sizeof(int), size * size, f);
    fclose(f);

    // here we read from this file and check values from three
    int check_as[size][size];
    f = fopen("./out_fragment", "rb");
    if (f == NULL)
    {
        return 2;
    }
    size_t check = fread(check_as, sizeof(int), size*size, f);
    for (size_t ix=0; ix<size; ix++)
    {
        for (size_t jx=0; jx<size; jx++)
        {
            printf("%d ", check_as[ix][jx]);
        }
        printf("\n");
    }
    fclose(f);

    // here we free this matrix
    for (size_t ix=0; ix<size; ix++)
    {
        free(as[ix]);
    }
    free(as);
}