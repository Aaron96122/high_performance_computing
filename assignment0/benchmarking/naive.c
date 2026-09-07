#include <stdio.h>
#include <time.h>

int main(void)
{
    double result=0;
    clock_t tic = clock();
    for (int i=0; i<1000000000; i++)
    {
        result+=i;
    }
    clock_t toc = clock();
    printf("%f\n", result);
    printf("runtime of a single loop iteration is around: %f\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    return 0;
}