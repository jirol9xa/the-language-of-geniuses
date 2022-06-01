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

    struct Out_str
    {
        char *str;
        int   size;
        int   capacity;
    };


    #define WORD(param)     text->words[param].word
    #define WORD_LEN(param) text->words[param].length
    #define PRINT_LINE fprintf(stderr, "[%s:%d]\n", __func__, __LINE__);


    int makeTrans(FILE *src, FILE *dst);
    
    int GenSemicolon(Text *text, int i, Out_str *dst);
    int GenOpenBrk  (Text *text, int i, Out_str *dst);
    int GenCloseBrk (Text *text, int i, Out_str *dst);
    int GenEqual    (Text *text, int i, Out_str *dst);
    int GenOne      (Text *text, int i, Out_str *dst);
    int GenTwo      (Text *text, int i, Out_str *dst);
    int GenMain     (Text *text, int i, Out_str *dst);
    int GenAdd      (Text *text, int i, Out_str *dst);
    int GenIf       (Text *text, int i, Out_str *dst);
    int GenZero     (Text *text, int i, Out_str *dst);
    int GenMul      (Text *text, int i, Out_str *dst);
    int GenSub      (Text *text, int i, Out_str *dst);
    int GenDefine   (Text *text, int i, Out_str *dst);
    int GenGE       (Text *text, int i, Out_str *dst);
    int GenLE       (Text *text, int i, Out_str *dst);
    int GenG        (Text *text, int i, Out_str *dst);
    int GenL        (Text *text, int i, Out_str *dst);
    int GenRet      (Text *text, int i, Out_str *dst);
    int GenE        (Text *text, int i, Out_str *dst);
    int GenOpenRnd  (Text *text, int i, Out_str *dst);
    int GenCloseRnd (Text *text, int i, Out_str *dst);
    int GenScanf    (Text *text, int i, Out_str *dst);
    int GenPrintf   (Text *text, int i, Out_str *dst);
    int writeVar    (int number, Out_str *dst);


    int out_strResize(Out_str *dst);

#endif