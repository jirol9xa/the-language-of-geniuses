#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../Include/Trans.h"


static int textCtor  (Text *text, int capacity);
static int textDtor  (Text *text);
static int textResize(Text *text);

static int tableDtor (Name_table *table);
static int tableCtor (Name_table *table);
static int nameSearch(Name_table *table, Word *word);
static int nameInsert(Name_table *table, Word *word);

static int out_strCtor(Out_str *dst);
static int out_strDtor(Out_str *dst);

static long fileLength(FILE *fp);
static int  myStrCmp  (char *src, char *new_code);
static int  getWords  (char *full_text, Text *text, int length);


// int main()
// {
//     FILE *src = fopen("text_test.txt", "r");
//     FILE *dst = fopen("dst.txt", "w");

//     makeTrans(src, dst);

//     return 0;
// }


int makeTrans(FILE *src, FILE *dst)
{
    assert(src);
    assert(dst);

    long length = fileLength(src);

    char *full_text = (char *) calloc(length + 1, sizeof(char));
    fread(full_text, length, sizeof(char), src);
    full_text[length] = '\0';

    Text text = {};
    textCtor(&text, length / 4);

    getWords(full_text, &text, length + 1);

    Name_table table = {};
    tableCtor(&table);

    Out_str dst_str = {};
    out_strCtor(&dst_str);

    #define COMPARE(param) !strcmp(word, param)

    for (int i = 0; i < text.size; ++i)
    {
        char *word   = text.words[i].word;
        int   length = text.words[i].length;
    
        if      (COMPARE("фупм"))       i += GenSemicolon(&text, i, &dst_str);
        else if (COMPARE("от"))         i += GenOpenBrk  (&text, i, &dst_str);  
        else if (COMPARE("не"))         i += GenCloseBrk (&text, i, &dst_str);
        else if (COMPARE("это"))        i += GenEqual    (&text, i, &dst_str);
        else if (COMPARE("литр"))       i += GenOne      (&text, i, &dst_str);
        else if (COMPARE("двулик"))     i += GenTwo      (&text, i, &dst_str);
        else if (COMPARE("залететь"))   i += GenDefine   (&text, i, &dst_str);
        else if (COMPARE("нк"))         i += GenMain     (&text, i, &dst_str);
        else if (COMPARE("смешать"))    i += GenAdd      (&text, i, &dst_str);
        else if (COMPARE("есть"))       i += GenGE       (&text, i, &dst_str);
        else if (COMPARE("если"))       i += GenIf       (&text, i, &dst_str);
        else if (COMPARE("уже"))        i += GenLE       (&text, i, &dst_str);
        else if (COMPARE("еще"))        i += GenG        (&text, i, &dst_str);
        else if (COMPARE("исчерпать"))  i += GenL        (&text, i, &dst_str);
        else if (COMPARE("пора"))       i += GenRet      (&text, i, &dst_str);
        else if (COMPARE("ничто"))      i += GenZero     (&text, i, &dst_str);
        else if (COMPARE("почти"))      i += GenE        (&text, i, &dst_str);
        else if (COMPARE("сходить"))    i += GenOpenRnd  (&text, i, &dst_str);
        else if (COMPARE("написать"))   i += GenCloseRnd (&text, i, &dst_str);
        else if (COMPARE("запить"))     i += GenMul      (&text, i, &dst_str);
        else if (COMPARE("отпить"))     i += GenSub      (&text, i, &dst_str);
        else if (COMPARE("придумать"))  i += GenScanf    (&text, i, &dst_str);
        else if (COMPARE("проснуться")) i += GenPrintf   (&text, i, &dst_str);
        else    // if not key word  ==> variable
        {
            Word *word_ptr = &text.words[i];
            int number = nameSearch(&table, word_ptr);

            if (number == -1)   
            {
                nameInsert(&table, word_ptr);
                number = table.size - 1;
            }
            PRINT_LINE;
            fprintf(stderr, "num = %d\n", number);
            writeVar(number, &dst_str);
        }
    }

    #undef COMPARE

    if (dst_str.size >= dst_str.capacity - 2)   out_strResize(&dst_str);

    dst_str.str[dst_str.size++] = '$';
    dst_str.str[dst_str.size] = '\0';

    fwrite(dst_str.str, sizeof(char), dst_str.size, dst);

    fprintf(stderr, "code = %s", dst_str.str);

    textDtor(&text);
    tableDtor(&table);
    out_strDtor(&dst_str);
    free(full_text);
    return 0;
}


static int skipSpace(char *buff)
{
    assert(buff);

    int i = 0; 

    while (buff[i] == ' ' || buff[i] == '\n')   buff[i++] = '\0';

    return i;
}


static int getWords(char *full_text, Text *text, int length)
{
    assert(full_text);
    assert(text);

    WORD(text->size++) = full_text;

    for (int i = 0; i < length; ++i)
    {
        char symb = full_text[i];
        if (symb != ' ' && symb != '\n')   continue;

        int space_amnt = skipSpace(full_text + i);

        if (text->size > text->capacity - 1)    textResize(text);

        WORD_LEN(text->size - 1) = full_text + i - WORD(text->size - 1);

        i += space_amnt - 1;
        // full_text[i] = '\0';
        
        if  (full_text[i + 1] != '\0') WORD(text->size++) = full_text + i + 1;
    }

    return 0;
}


static int textCtor(Text *text, int capacity)
{
    assert(text);

    text->words      = (Word *) calloc(32, sizeof(Word));
    assert(text->words);

    text->capacity = 32;

    return 0;
}


static int textDtor(Text *text)
{
    assert(text);

    text->size     = 0;
    text->capacity = 0;
    free(text->words);

    return 0;
}


static int textResize(Text *text)
{
    assert(text);

    text->capacity = !text->capacity + text->capacity * 2;

    void *temp_ptr = realloc(text->words, text->capacity * sizeof(Word));
    assert(temp_ptr);

    text->words = (Word *) temp_ptr;

    return 0;
}


static long fileLength(FILE *fp)
{
    assert(fp);

    long length = 0;

    #define FSEEK(param)    fseek(fp, 0L, SEEK_##param);

    FSEEK(END);
    
    length = ftell(fp);
    
    FSEEK(SET);

    #undef FSEEK

    return length;
}


static int myStrCmp(char *src, char *new_code)
{
    assert(src);
    assert(new_code);

    return 0;
}


static int tableCtor(Name_table *table)
{
    assert(table);

    table->word = (Word *) calloc(8, sizeof(Word));
    assert(table->word);

    table->capacity = 8;

    return 0;
}


static int tableDtor(Name_table *table)
{
    assert(table);

    free(table->word);
    return 0;
}


static int tableResize(Name_table *table)
{
    assert(table);

    table->capacity = !table->capacity + table->capacity * 2;

    void *temp_ptr = realloc(table->word, table->capacity * sizeof(Word));
    assert(temp_ptr);

    table->word = (Word *) temp_ptr;

    return 0;
}


static int nameInsert(Name_table *table, Word *word)
{
    assert(table);
    assert(word);

    if (table->size >= table->capacity - 1)  tableResize(table);

    table->word[table->size].word = word->word;
    table->word[table->size++].length = word->length;

    return 0;
}


static int nameSearch(Name_table *table, Word *word)
{
    assert(table);
    assert(word);

    for (int i = 0; i < table->size; ++i)
    {
        if (word->length == table->word[i].length)
        {
            if (!strcmp(table->word[i].word, word->word))   return i;
        }
    }

    return -1;
}


static int out_strCtor(Out_str *dst)
{
    assert(dst);

    dst->capacity = 64;
    dst->str      = (char *) calloc(dst->capacity, sizeof(char));

    return 0;
}


static int out_strDtor(Out_str *dst)
{
    assert(dst);

    dst->capacity = 0;
    free(dst->str);

    return 0;
}


int out_strResize(Out_str *dst)
{
    assert(dst);

    dst->capacity = !dst->capacity + dst->capacity * 2;

    void *temp_ptr = realloc(dst->str, dst->capacity * sizeof(char));
    assert(temp_ptr);

    dst->str = (char *) temp_ptr;

    return 0;
}
