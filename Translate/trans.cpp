#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../Include/Trans.h"


static int textCtor(Text *text, int capacity);
static int textDtor(Text *text);
static int textResize(Text *text);

static int tableDtor(Name_table *table);
static int tableCtor(Name_table *table);

static long fileLength(FILE *fp);
static int  myStrCmp  (char *src, char *new_code);
static int  getWords  (char *full_text, Text *text, int length);


int main()
{
    FILE *src = fopen("text_test.txt", "r");
    FILE *dst = fopen("dst.txt", "w");

    makeTrans(src, dst);

    return 0;
}


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

    for (int i = 0; i < text.size; ++i)
    {
        char *word   = text.words[i].word;
        int   length = text.words[i].length;
    
        if      (!strcmp(word, "фупм"))  i += GenSemicolon(&text, i);
        else if (!strcmp(word, "от"))    i += GenOpenBrk(&text, i);  
        else if (!strcmp(word, "не"))    i += GenCloseBrk(&text, i);

        printf("i = %d\n", i);
    }

    // for (int i = 0; i < text.size; ++i)
    // {
    //     if (*((int *) text.words[i].word) == 'Ф')   printf("Ф is here\n");
    // }

    free(full_text);
    return 0;
}


int writeTranslit(Word *src, char *dst)
{
    assert(src);
    assert(dst);

    char *word = src->word;

    for (int i = 0; i < src->length; i += 2)
    {
        dst = (word[i] == 0xd0) * ()
    }

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

    if (table->size > table->capacity - 1)  tableResize(table);

    table->word[table->size].word = word->word;
    table->word[table->size++].length = word->length;

    return 0;
}


static int nameSearch(Name_table *table, Word *word)
{
    assert(table);
    assert(word);

    
}