#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "../Include/Trans.h"


#define COMPARE(param) !strcmp(WORD(i + (++offset)), param)


int GenSemicolon(Text *text, int i)//, char *dst);
{
    assert(text);

    int offset = 0;

    if (COMPARE("гавно"))
    {
        fprintf(stderr, "Фупм гавно!!!\n");
        return offset;
    }

    return 0;
}


int GenOpenBrk(Text *text, int i)
{
    assert(text);

    PRINT_LINE;

    int offset = 0;

    if (COMPARE("коробки") && COMPARE("до") && COMPARE("нк"))
    {
       fprintf(stderr, "От коробки до нк не сосет фртк!!!!!\n");
       return offset;
    }

    return 0;
}


int GenCloseBrk(Text *text, int i)
{
    assert(text);

    int offset = 0;

    if (COMPARE("сосет") && COMPARE("фртк"))
    {
        fprintf(stderr, "Не сосет фртк!!!!\n");
        return offset;
    }

    return 0;
}