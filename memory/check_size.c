#include <stdio.h>

int main(int argc,char *argv[])
{
    printf("size of char is: %zu\n", sizeof(char));
    printf("size of short int is: %zu\n", sizeof(short int));
    printf("size of int is: %zu\n", sizeof(int));
    printf("size of long int is: %zu\n", sizeof(long int));

    printf("size of float is: %zu\n", sizeof(float));
    printf("size of double is %zu\n", sizeof(double));

    printf("size of unsigned int is: %zu\n", sizeof(unsigned int));
    printf("size of unsigned long int is : %zu\n", sizeof(unsigned long int));

    printf("size of pointer is: %zu\n", sizeof(void*));
    printf("\n");

    {char a;
    printf("size of char is: %zu\n", sizeof(a));
    }

    {int a;
    printf("size of pointer is: %zu\n", sizeof(&a));
    return 0;
    }
}
