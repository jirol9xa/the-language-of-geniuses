#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "LogsLib.h"
#include "Tree.h"
#include "TextLib.h"
#include "Suff_tree.h"
#include "Recursive_descent.h"
#include "Syntax.h"

static int lexical(Tokens *tokens, char *string);
//static int lexAnal(Tokens *tokens);
static int tokensCtor(Tokens *tokens);
static Node *number();
static Node *identific();
static Node *oper();
static int SyntaxError();
static int tokensResize(Tokens *tokens);
static int lexAnal(Tokens *tokens);

static Node *GetGeneral(Tokens *tokens);
static Node *GetNumber(Tokens *tokens, int *iter);
static Node *GetPriority(Tokens *tokens, int *iter);

extern const char *RESERVED_WORDS;
extern const int LETTERS_AMOUNT;

extern int IS_IF;
extern int IS_WHILE;
extern int IS_RETURN;
extern int IS_CONST;
extern int IS_MAIN;
extern int IS_DEFINE;

extern int IS_SIN;
extern int IS_COS;
extern int IS_LN;

char *STRING = nullptr;

int constructTree(Tree *tree, char *string)
{
    assert(string);
    assert(tree);
    if (tree->status.destructed_tree)
    {
        printf("!!! ERROR Can't work with destructed tree !!!\n");
        return -1;
    }

    Tokens tokens = {};
    tokensCtor(&tokens);

    lexical(&tokens, string);

    //tree->root = GetGeneral(&tokens);

    PRINT_LINE;
    for (int i = 0; i < tokens.size; i++)
    {
        PRINT_LINE;
        printf("tkn = %p\n", tokens.array[i]);
        if (!tokens.array[i]->node_type.bytes.is_number)
        {
            PRINT_LINE;
            printf("str = %s\n", tokens.array[i]->value.str);
            PRINT_LINE;
            free(tokens.array[i]->value.str);
        }
        PRINT_LINE;
        free(tokens.array[i]);
    }
    free(tokens.array); // delete this!!!
    return 0;
}

static int lexical(Tokens *tokens, char *string)
{
    assert(tokens);
    assert(string);

    STRING = string;
    int is_ok = 0; // is_ok = 0

    while (*STRING != '$')
    {
        is_ok = 1;

        if (tokens->size + 1 > tokens->capacity) // resizing tokens
        {
            tokens->capacity *= 2;
            void *temp_ptr = (Node **)realloc(tokens->array, sizeof(Node *) * tokens->capacity);
            if (temp_ptr)
            {
                tokens->array = (Node **)temp_ptr;
            }
            else
            {
                printf("!!! ERROR Can't allocate memory !!!\n");
                return -1;
            }
        }

        if (*STRING >= '0' && *STRING <= '9')
        {
            tokens->array[tokens->size] = number();
            tokens->size++;
        }
        else if (*STRING >= 'a' && *STRING <= 'z')
        {
            tokens->array[tokens->size] = identific();
            tokens->size++;
        }
        else
        {
            tokens->array[tokens->size] = oper(); // including (\n, \r ...)
            tokens->size++;
        }
    }

    PRINT_LINE;
    // adding a key character

    if (is_ok)
    {
        PRINT_LINE;
        if (tokens->size + 1 > tokens->capacity)
        {
            tokens->capacity *= 2;
            void *temp_ptr = (Node **)realloc(tokens->array, sizeof(Node *) * tokens->capacity);
            if (temp_ptr)
            {
                tokens->array = (Node **)temp_ptr;
            }
            else
            {
                printf("!!! ERROR Can't allocate memory !!!\n");
                return -1;
            }
        }

        Node **last_node = &(tokens->array[tokens->size]);

        lexAnal(tokens);

        *last_node = (Node *)calloc(1, sizeof(Node));
        (*last_node)->node_type.bytes.is_operator = 1;
        
        (*last_node)->value.str = (char *) calloc(2, sizeof(char));
        (*last_node)->value.str[0] = '$';
        (*last_node)->value.str[1] = '\0';

        tokens->size++;

        return 0;
    }

    SyntaxError();
    return -1;
}

//static int lexAnal(Tokens *tokens)
//{
//    assert(tokens);
//
//    Suff_Tree *suff_tree = suffTreeCtor();
//
//    for (int i = 0; i < tokens->size; i++)
//    {
//
//    }
//    return 0;
//}

static int tokensCtor(Tokens *tokens)
{
    assert(tokens);

    tokens->array = (Node **)calloc(1, sizeof(Node *));
    tokens->capacity = 1;

    return 0;
}

static int tokensDtor(Tokens *tokens)
{
    assert(tokens);

    free(tokens);
    return 0;
}

static Node *number()
{
    Node *node = (Node *)calloc(1, sizeof(Node));
    double value = 0;

    while (*STRING >= '0' && *STRING <= '9')
    {
        value = value * 10 + (*STRING - '0');
        STRING++;
    }

    STRING += skipSpace(STRING);

    node->node_type.bytes.is_number = 1;
    node->value.number = value;

    return node;
}

static Node *identific()
{
    Node *node = (Node *)calloc(1, sizeof(Node));
    char buffer[33] = {};

    sscanf(STRING, "%[a-z,0-9,-,_]s", buffer);

    STRING += strlen(buffer);

    STRING += skipSpace(STRING);

    //for (int i = 0; i < 19 && *STRING >= 'a' && *STRING <= 'z'; i++)
    //{
    //    buffer[i] = *STRING++;
    //}

    //if (strlen(buffer) == 1)
    //{
    //    node->value.str = (char *) calloc(2, sizeof(char));
    //    node->node_type.bytes.is_variable = 1;
    //    node->value.str[0] = buffer[0];
    //    node->value.str[1] = '\0';
    //
    //    return node;
    //}

    //node->node_type.bytes.is_func = 1;
    node->value.str = (char *) calloc(strlen(buffer) + 1, sizeof(char));
    node->node_type.bytes.is_variable = 1;
    strcpy(node->value.str, buffer);

    return node;

    //if (strstr("sincosln", node->value.str))
    //{
    //    switch (node->value.str[0])
    //    {
    //        case 's':
    //            if (node->value.str[1] == 'i')
    //            {
    //                node->node_type.bytes.is_func = IS_SIN;
    //                return node;
    //            }
    //            //node->node_type.bytes.is_func = IS_SCAN;
    //            return node;
    //        case 'c':
    //            node->node_type.bytes.is_func = IS_COS;
    //            return node;
    //        case 'l':
    //            node->node_type.bytes.is_func = IS_LN;
    //            return node;
    //        case 'p':
    //            //node->node_type.bytes.is_func = IS_PRINT;
    //            return node;
    //    }
    //}

    //SyntaxError();
    return nullptr;
}

static Node *oper()
{
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->value.str = (char *)calloc(3, sizeof(char));

    node->value.str[0] = *STRING++;
    node->value.str[1] = '\0';

    if (!strchr("<>=", node->value.str[0]))
    {
        if (*STRING == '=')
        {
            node->value.str[1] = *STRING++;
            node->value.str[2] = '\0';
        }
    }

    STRING += skipSpace(STRING);

    node->node_type.bytes.is_operator = 1;

    if (!strchr("+-/*^()$>=<", node->value.str[0]))
    {
        PRINT_LINE;
        SyntaxError();
    }

    return node;
}

static int SyntaxError()
{
    printf("!!! ERROR SyntaxError !!!\n");
    exit(1);
}

static Node *GetGeneral(Tokens *tokens)
{
    assert(tokens);

    int iter = 0;

    Node *root = GetAddSub(tokens, &iter); // change to statement

    if (tokens->array[iter]->node_type.bytes.is_operator && tokens->array[iter]->value.symbol == '$')
    {
        iter++;
    }
    else
    {
        SyntaxError();
    }

    free(tokens->array[iter -1]);
    return root;
}

//static Node *GetMulDiv(Tokens *tokens, int *iter)
//{
//    assert(tokens);
//    assert(iter);
//
//    Node *oper = nullptr;
//
//    Node *value = GetPriority(tokens, iter);
//
//    while (tokens->array[*iter]->node_type.bytes.is_operator && strchr("*^/", tokens->array[*iter]->value.symbol))
//    {
//        oper = tokens->array[*iter];
//        oper->left_child = value;
//        (*iter)++;
//
//        oper->right_child = GetPriority(tokens, iter);
//
//        value = oper;
//    }
//
//    return value;
//}

//static Node *GetPriority(Tokens *tokens, int *iter)
//{
//    assert(tokens);
//    assert(iter);
//
//    if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.symbol == '(')
//    {
//        (*iter)++;
//        free(tokens->array[*iter - 1]);
//
//        Node *value = GetAddSub(tokens, iter);
//
//        if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.symbol == ')')
//        {
//            (*iter)++;
//            free(tokens->array[*iter - 1]);
//            return value;
//        }
//
//        SyntaxError();
//    }
//    else
//    {
//        return GetNumber(tokens, iter);
//    }
//
//    return nullptr;
//}
//
//
//static Node *GetNumber(Tokens *tokens, int *iter)
//{
//    assert(tokens);
//    assert(iter);
//
//    if (tokens->array[*iter]->node_type.bytes.is_number || tokens->array[*iter]->node_type.bytes.is_variable)
//    {
//        (*iter)++;
//
//        return tokens->array[*iter - 1];
//    }
//    else if (tokens->array[*iter]->node_type.bytes.is_func)
//    {
//        Node *func = GetFunc(tokens, iter);
//        return func;
//    }
//
//    SyntaxError();
//    return nullptr;
//}


static int lexAnal(Tokens *tokens)
{
    assert(tokens);

    Suff_Tree *suff_tree = suffTreeCtor();

    for (int i = 0; i < tokens->size; i++)
    {
        Node *token = tokens->array[i];

        if (token->node_type.bytes.is_variable)
        {
            int status = isKeyword(token->value.str, suff_tree);

            token->node_type.number = status + !status;
        }
    }

    for (int i = 0; i < tokens->size; i++)
    {
        printf("str = %s, status = %d\n", tokens->array[i]->value.str, tokens->array[i]->node_type.number);
    }

    suffTreeDtor(suff_tree);
    return 0;
}
