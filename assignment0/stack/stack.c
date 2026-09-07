#include <stdio.h>
int main(void)
{
    int size = 1000000;
    int as[size];
    
    for (size_t ix=0; ix < size; ix++)
    {
        as[ix] = 0;
    }
    printf("%d\n", as[0]);
    printf("%zu\n", sizeof(int));
}
// here I have some questions:
// 1. what's the size_t? standard type in c programming?
// 2. how to check the so-called memory size of different systems. 


// Here is my explanation of why the segmentation fault appears as: 11 when the size 
// is extremely large like: 10000000
// 1. Each element of array as is an integer when the sizeof(int)=4bytes
// The system setting for stack memory is 8MB, which is 8,000,000 bytes
// Thus, the maximum space for this array stroing integers theoretically is 8,000,000 / 4 = 2,000,000 integers
// Meanwhile, some spaces are also reserved and used for function calls, etc.
// Therefore, when the size exceeds the maximum storage, there would be the segmention fault, whic is normally correlated with memory overflow, memory leakage, etc.
// And the 11 here for the segmentation means invalid memory access. 