#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "LogsLib.h"
#include "Tree.h"
#include "Recursive_descent.h"


static int   lexical(Tokens *tokens, char *string);
static int   lexAnal(Tokens *tokens);
static int   tokensCtor(Tokens *tokens);
static Node *number();
static Node *identific();
static Node *oper();
static int   SyntaxError();
static int   tokensResize(Tokens *tokens);

static Node *GetGeneral (Tokens *tokens);
static Node *GetNumber  (Tokens *tokens, int *iter);
static Node *GetMulDiv  (Tokens *tokens, int *iter);
static Node *GetPriority(Tokens *tokens, int *iter);
static Node *GetNumber  (Tokens *tokens, int *iter);
static Node *GetAddSub  (Tokens *tokens, int *iter);
static Node *GetFunc    (Tokens *tokens, int *iter);


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

    tree->root = GetGeneral(&tokens);

    free(tokens.array); // delete this!!!
    return 0;
}


static int lexical(Tokens *tokens, char *string)
{
    assert(tokens);
    assert(string);

    STRING = string;
    int is_ok = 0;

    while (*STRING != '$')
    {
        is_ok = 1;

        if (tokens->size + 1 > tokens->capacity)    // resizing tokens
        {
            tokens->capacity *= 2;
            void *temp_ptr = (Node **) realloc(tokens->array, sizeof(Node *) * tokens->capacity);
            if (temp_ptr)
            {
                tokens->array = (Node **) temp_ptr;
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

    // adding a key character

    if (is_ok)
    {
        if (tokens->size + 1 > tokens->capacity)
        {
            tokens->capacity *= 2;
            void *temp_ptr = (Node **) realloc(tokens->array, sizeof(Node *) * tokens->capacity);
            if (temp_ptr)
            {
                tokens->array = (Node **) temp_ptr;
            }
            else
            {
                printf("!!! ERROR Can't allocate memory !!!\n");
                return -1;
            }
        }

        tokens->array[tokens->size] = (Node *) calloc(1, sizeof(Node));
        tokens->array[tokens->size]->node_type.bytes.is_operator = 1;
        tokens->array[tokens->size]->value.symbol                = '$';
        tokens->size++;


        return 0;
    }

    SyntaxError();
    return -1;
}


static int   lexAnal(Tokens *tokens)
{
    assert(tokens);

    

    return 0;
}


static int tokensCtor(Tokens *tokens)
{
    assert(tokens);

    tokens->array = (Node **) calloc(1, sizeof(Node *));
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
    Node *node = (Node *) calloc(1, sizeof(Node));
    double value = 0;

    while(*STRING >= '0' && *STRING <= '9')
    {
        value = value * 10 + (*STRING - '0');
        STRING++;
    }

    node->node_type.bytes.is_number = 1;
    node->value.number = value;

    return node;
}


static Node *identific()
{
    Node *node = (Node *) calloc(1, sizeof(Node));
    char buffer[20] = {};

    for (int i = 0; i < 19 && *STRING >= 'a' && *STRING <= 'z'; i++)
    {
        buffer[i] = *STRING++;
    }

    if (strlen(buffer) == 1)
    {
        node->node_type.bytes.is_variable = 1;
        node->value.symbol = buffer[0];

        return node;
    }

    node->node_type.bytes.is_func = 1;

    node->value.func = (char *) calloc(strlen(buffer) + 1, sizeof(char));
    strcpy(node->value.func, buffer);

    if (strstr("sincosln", node->value.func))
    {
        switch (node->value.func[0])
        {
            case 's':
                node->node_type.bytes.is_sin = 1;
                return node;
            case 'c':
                node->node_type.bytes.is_cos = 1;
                return node;
            case 'l':
                node->node_type.bytes.is_ln = 1;
                return node;
        }
    }

    SyntaxError();
    return nullptr;
}


static Node *oper()
{
    Node *node = (Node *) calloc(1, sizeof(Node));

    node->value.symbol = *STRING++;
    node->node_type.bytes.is_operator = 1;

    if (!strchr("+-/*^()$", node->value.symbol))
    {
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

    Node *root = GetAddSub(tokens, &iter);

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


static Node *GetMulDiv(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *oper = nullptr;
    
    Node *value = GetPriority(tokens, iter);

    while (tokens->array[*iter]->node_type.bytes.is_operator && strchr("*^/", tokens->array[*iter]->value.symbol))
    {
        oper = tokens->array[*iter];
        oper->left_child = value;
        (*iter)++;

        oper->right_child = GetPriority(tokens, iter);

        value = oper;
    }

    return value;
}


static Node *GetPriority(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.symbol == '(')
    {
        (*iter)++;
        free(tokens->array[*iter - 1]);

        Node *value = GetAddSub(tokens, iter);

        if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.symbol == ')')
        {
            (*iter)++;
            free(tokens->array[*iter - 1]);
            return value;
        }

        SyntaxError();
    }
    else
    {
        return GetNumber(tokens, iter);
    }

    return nullptr;
}


static Node *GetNumber(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (tokens->array[*iter]->node_type.bytes.is_number || tokens->array[*iter]->node_type.bytes.is_variable)
    {   
        (*iter)++;

        return tokens->array[*iter - 1];
    }
    else if (tokens->array[*iter]->node_type.bytes.is_func)
    {
        Node *func = GetFunc(tokens, iter);
        return func;
    }
    
    SyntaxError();
    return nullptr;
}


static Node *GetFunc(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *func = tokens->array[*iter];
    (*iter)++;
    
    func->left_child = GetPriority(tokens, iter);

    return func;
}


static Node *GetAddSub(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *oper = nullptr;
    Node *value = GetMulDiv(tokens, iter);

    while (tokens->array[*iter]->node_type.bytes.is_operator && strchr("+-", tokens->array[*iter]->value.symbol))
    {
        oper = tokens->array[*iter];
        oper->left_child = value;
        (*iter)++;

        oper->right_child = GetMulDiv(tokens, iter);

        value = oper;
    }    

    return value;
}