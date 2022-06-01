#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "../Include/Trans.h"


#define COMPARE(param) !strcmp(WORD(i + (++offset)), param)
#define WRITE_CODE(param)                   \
{                                           \
    dst->size += strlen(param);             \
    if (dst->size >= dst->capacity - 1)     \
        {                                   \
            out_strResize(dst);             \
        }                                   \
                                            \
    strcat(dst->str, param);                \
}

#define MAKE_FUNC(name, code)                   \
int Gen##name(Text *text, int i, Out_str *dst)   \
{                                               \
    assert(text);                               \
    assert(dst);                                \
                                                \
    WRITE_CODE(code);                           \
                                                \
    return 0;                                   \
}


int GenSemicolon(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("гавно"))
    {
        fprintf(stderr, "Фупм гавно!!!\n");

        WRITE_CODE(";\n");
        return offset;
    }

    return 0;
}


int GenOpenBrk(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    PRINT_LINE;

    int offset = 0;

    if (COMPARE("коробки") && COMPARE("до") && COMPARE("нк"))
    {
        fprintf(stderr, "От коробки до нк не сосет фртк!!!!!\n");

        WRITE_CODE("{\n");
        return offset;
    }

    return 0;
}


int GenCloseBrk(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("сосет") && COMPARE("фртк"))
    {
        fprintf(stderr, "Не сосет фртк!!!!\n");

        WRITE_CODE("}\n");
        return offset;
    }

    return 0;
}


MAKE_FUNC(Equal, "= ")
MAKE_FUNC(One,   "1 ")
MAKE_FUNC(Two,   "2 ")
MAKE_FUNC(Main,  "main ")
MAKE_FUNC(Add,   "+ ")
MAKE_FUNC(If,    "if ")
MAKE_FUNC(Zero,  "0 ")
MAKE_FUNC(Mul,   "* ")
MAKE_FUNC(Sub,   "- ")


int GenDefine(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);


    int offset = 0;

    if (COMPARE("на"))
    {
        WRITE_CODE("define ");
        return offset;
    }

    return 0;
}


int GenGE(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("хоть") && COMPARE("немного"))
    {
        WRITE_CODE(">= ");
        return offset;
    }

    return 0;
}


int GenLE(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("кончислось(("))
    {
        WRITE_CODE("<= ");
        return offset;
    }

    return 0;
}


int GenG(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("осталось"))
    {
        WRITE_CODE("> ");
        return offset;
    }

    return 0;
}


int GenL(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("не") && COMPARE("осталось"))
    {
        WRITE_CODE("< ");
        return offset;
    }

    return 0;
}


int GenRet(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("спать"))
    {
        WRITE_CODE("return ");
        return offset;
    }

    return 0;
}


int GenE(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("то") && COMPARE("же") && COMPARE("самое"))
    {
        WRITE_CODE("== ");
        return offset;
    }

    return 0;
}


int GenOpenRnd(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("в") && COMPARE("ардос"))
    {
        WRITE_CODE("( ");
        return offset;
    }

    return 0;
}


int GenCloseRnd(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("объесос"))
    {
        WRITE_CODE(") ");
        return offset;
    }

    return 0;
}


int GenScanf(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("повод") && COMPARE("для"))
    {
        WRITE_CODE("scanf ");
        return offset;
    }

    return 0;
}


int GenPrintf(Text *text, int i, Out_str *dst)
{
    assert(text);
    assert(dst);

    int offset = 0;

    if (COMPARE("и"))
    {
        WRITE_CODE("printf ");
        return offset;
    }

    return 0;
}


int writeVar(int number, Out_str *dst)
{
    assert(dst);

    char buff[32] = {};
    sprintf(buff, "a%d ", number);

    WRITE_CODE(buff);

    return 0;
}