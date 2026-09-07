#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int size = 1000000000;
    int *as = malloc(size * sizeof(int));
    
    if (as == NULL)
    {
        return 1;
    }

    for (size_t ix=0; ix<size; ix++)
    {
        as[ix] = 0;
    }
    printf("%d\n", as[0]);
    free(as);
}
// The heap memory is not unlimited, and it is limited by: available physical RAM,
// available vritual memory, operating-system limits, other programs using memory, 
// processor's address space, the requested allocation size. 
// My questio here is:
// Would you introduce the memory mechanism in operating systems, like where the stack is, where the heap is...
