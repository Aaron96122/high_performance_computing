#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int size=10;

    int *ascentries = malloc(sizeof(int) * size*size);
    if (ascentries == NULL)
    {
        return 1;
    }
    int **as = malloc(sizeof(int*) * size);
    
    // here we initialize the matrix
    for (size_t ix=0, jx=0; ix<size; ix++, jx+=size)
    {
        as[ix] = ascentries + jx;
    }

    // here we fulfill the matrix with values
    for (size_t ix=0; ix<size; ix++)
    {
        for (size_t jx=0; jx<size; jx++)
        {
            as[ix][jx] = ix * jx;
            // printf("%d ", as[ix][jx]);
        }
        // printf("\n");
    }

    // here we write the matrix into the file out_heap_continuous
    FILE *f = fopen("./out_heap_continuous", "wb");
    if (f == NULL)
    {
        return 2;
    }
    /* alternative 1 : not work */
    // fwrite(as, sizeof(int), size*size, f);

    /* alternative 2 : work */
    // for (size_t ix=0; ix<size; ix++)
    // {
    //     fwrite(as[ix], sizeof(int), size*size, f);
    // }
    

    /* alternative 3 : work */
    fwrite(ascentries, sizeof(int), size*size, f);
    fclose(f);

    // here we read from the out_heap_continuous file and check values there
    int check_as[size][size];
    f = fopen("./out_heap_continuous", "rb");
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
    free(as);
    free(ascentries);
}