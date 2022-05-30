#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
    FILE *fp = fopen("text_test.txt", "r");

    long length = 0;

    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    fprintf(stderr, "length = %d\n", length);

    char buff[64] = {};

    fread(buff, sizeof(char), length, fp);
    buff[length] = '\0';

    for (int i = 0; i < length; ++i)
    {
        fprintf(stderr, "%x ", buff[i]);
        //if ()
    }    

    fprintf(stderr, "\n");

    fprintf(stderr, "%x ", 'а');
    fprintf(stderr, "%x ", 'б');
    fprintf(stderr, "%x ", 'в');
    fprintf(stderr, "%x ", 'г');

    fprintf(stderr, "\n");

    return 0;
}


static int writetransite(char *src)//char *dst)
{
    assert(src);

    int i = 0;

    //if ()

    return 0;  
}