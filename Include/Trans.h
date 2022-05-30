#ifndef TRANS_H
    #define TRANS_H

    struct Word
    {
        char *word;
        int   length;
        char *next;
    };

    struct Name_table
    {
        Word *word;
        int   size;
        int   capacity;
    };

    struct Text
    {
        Word *words;
        Name_table *name_table;
        int   capacity;
        int   size;
    };


    #define WORD(param)     text->words[param].word
    #define WORD_LEN(param) text->words[param].length
    #define PRINT_LINE fprintf(stderr, "[%s:%d]\n", __func__, __LINE__);


    int makeTrans(FILE *src, FILE *dst);
    
    int GenSemicolon(Text *text, int i);//, char *dst);
    int GenOpenBrk  (Text *text, int i);
    int GenCloseBrk(Text *text, int i);

#endif