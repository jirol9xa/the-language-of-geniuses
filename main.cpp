#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("!!! ERROR Can't run without sourse file !!!\n");
        return -1;
    }

    FILE *sourse = fopen(argv[1], "r");
    
    
    return 0;
}