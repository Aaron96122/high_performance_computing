#include <stdio.h>

// Here we include all the prototypes
int check_input_size(char* input);
char convert(char character);

int main(
    int argc,
    char *argv[]
)
{
    for (int ix=1; ix<argc; ix++)
    {
        // Here we define a temporary variable to store each input string
        char *tmp = argv[ix];
        int check_size = check_input_size(argv[ix]);
        if (check_size < 2)
        {
            printf("please provide appropriate arguments like: -a6 -b4");
            return 1;
        }
        else
        {
            char symbol = convert(tmp[check_size-2]);
            char number = tmp[check_size-1];
            printf("%c is %c ", symbol, number);
        }
        if (ix != argc-1)
        {
            printf("and ");
        }   
    }
    printf("\n");
}

int check_input_size(char* input)
{
    int i=0;
    while (input[i])
    {
        i++;
    }
    return i;
}

char convert(char character)
{
    int size = (int)character;
    if (size>=97 && size<=122)
    {
        size-=32;
        return (char)size;
    }
    else
    {
        return character;
    }
}
