#ifndef LEXER_H
    #define LEXER_H


    struct Tokens_t
    {
        Node **array;
        int capacity;
        int size;
    };


    int lexAnal(Tokens_t *tokens, Suff_Tree *suff_tree);
#endif