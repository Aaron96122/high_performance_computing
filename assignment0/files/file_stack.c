#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void)
{
    int size = 10;
    int as[size][size];

    // here we initialize matrix
    FILE *f = fopen("output_stack", "w");
    if (f == NULL)
    {
        return 1;
    }
    for (size_t ix=0; ix<size; ix++)
    {
        for (size_t jx=0; jx<size; jx++)
        {
            as[ix][jx] = ix * jx;
        }
        // fwrite(as[ix], sizeof(int), size, f);
    }
    /* alternative 1*/
    // fwrite(as, sizeof(int), size * size, f);
    
    /* alternative 2*/
    // for (size_t ix=0; ix<size; ix++)
    // {
    //     fwrite(as[ix], sizeof(int), size, f);
    // }

    /* alternative 3*/
    fwrite(as[0], sizeof(int), size*size, f);
    fclose(f);

    int check_as[size][size];
    f = fopen("./output_stack", "rb");
    if (f == NULL)
    {
        return 2;
    }
    size_t check = fread(check_as, sizeof(int), size * size, f);
    for (size_t ix=0; ix<size; ix++)
    {
        for (size_t jx=0; jx<size; jx++)
        {
                printf("%d\n", check_as[ix][jx]);
        }
    }  
    fclose(f);
}