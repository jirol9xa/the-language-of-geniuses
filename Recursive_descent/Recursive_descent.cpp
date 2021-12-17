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

static int lexical      (Tokens *tokens, char *string);
//static int lexAnal(Tokens *tokens);
static int tokensCtor   (Tokens *tokens);
static Node *number     ();
static Node *identific  ();
static Node *oper       ();
static int SyntaxError  ();
static int tokensResize (Tokens *tokens);
static int lexAnal      (Tokens *tokens);

static Node *GetGeneral (Tokens *tokens);
static Node *GetArg     (Tokens *tokens, int *iter);
static Node *GetPriority(Tokens *tokens, int *iter);

extern const char *RESERVED_WORDS;
extern const int LETTERS_AMOUNT;

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
    //
    //
    //
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
    //
    //
    //
    return 0;
}

/*!
    \brief  Func of lex anal))))
    \param  [Tokens *]tokens Pointer to tokens struct
    \param  [char *]string Array with code
    \return 0 if all right
*/
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

        (*last_node)->value.str = (char *)calloc(2, sizeof(char));
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
    node->value.str = (char *)calloc(strlen(buffer) + 1, sizeof(char));
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

    if (!strchr("+-/*^()$>=<{};,", node->value.str[0]))
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

    //Node *root = GetAddSub(tokens, &iter);  change to statement I don't know what exactly

    if (tokens->array[iter]->node_type.bytes.is_operator && tokens->array[iter]->value.str[0] == '$')
    {
        iter++;
    }
    else
    {
        SyntaxError();
    }

    free(tokens->array[iter - 1]);
    //return root;
}


/*!
    \brief  Function for reading external statement
    \param  [Tokens *]tokens Pointer to tokens
    \param  [int *]iter Pointer to current token
    \return [Node *] Pointer to new sub tree
*/
static Node *GetExternStatement(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *stmnt = (Node *) calloc(1, sizeof(Node));



}


static Node *GetStatement(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *stmnt = (Node *) calloc(1, sizeof(Node));

    //while
}

static Node *readStatement(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    #define IS_NUM(arg)     tokens->array[arg]->node_type.bytes.is_number
    #define IS_VAR(arg)     tokens->array[arg]->node_type.bytes.is_variable
    #define TYPE_BYTES(arg) tokens->array[arg]->node_type.bytes

    //if (tokens->size = *iter + 1)

    int stmnt_type = 0;
    int i = 0;

    while (IS_NUM(*iter + i) || IS_VAR(*iter + i))
    {
        if (IS_NUM(*iter + 1) && IS_NUM(*iter + i + 1) || IS_VAR(*iter + i) && IS_VAR(*iter + i + 1))
        {
            SyntaxError();
        }

        i++;
    }

    if (TYPE_BYTES(*iter + i).is_keyword)
    {
        // add creating if, while
    }
    else if (TYPE_BYTES(*iter + i).is_operator)
    {
        // add creating operations
    }
    else if (TYPE_BYTES(*iter + i).is_func)
    {
        // add creating functions
    }
    else               // TODO:
    {                  // Delete
        PRINT_LINE;    // this
        SyntaxError(); // ssssshhhhhh
    }                  // it

#undef IS_NUM
#undef IS_VAR
#undef TYPE_BYTES
}


static Node *GetWhile(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *while_ = tokens->array[*iter++];

    while_->left_child  = GetOperator(tokens, iter);
    while_->right_child = GetStatement(tokens, iter); // mb bulshit

    return while_;
}


static Node *GetIf(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *if_ = tokens->array[*iter++];

    if_->left_child = GetOperator(tokens, iter);

}


/*!
    \brief  Function that building sub tree for (<>=+*-/)
    \param  [Tokens *]tokens Pointer to tokens
    \param  [int *]iter Position of current token
    \return Pointer on sub tree 
*/
static Node *GetOperator(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (!tokens->array[*iter + 1]->node_type.bytes.is_operator || *iter >= (tokens->size - 3))
    {
        SyntaxError();
    }

    Node *l_val = tokens->array[*iter++];
    if (!l_val->node_type.bytes.is_variable || !l_val->node_type.bytes.is_number)
    {
        SyntaxError();
    }

    Node *oper  = tokens->array[*iter++];

    Node *r_val = tokens->array[*iter++];
    if (!r_val->node_type.bytes.is_variable || !r_val->node_type.bytes.is_number)
    {
        SyntaxError();
    }

    oper->left_child  = l_val;
    oper->right_child = r_val;

    return oper;
}


static Node *GetLogicOper(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *logic_oper = nullptr;
    Node *l_value = GetPriority(tokens, iter);
    Node *node = tokens->array[*iter];

    while (node->node_type.bytes.is_operator && strchr(">==<=", node->value.str[0]))
    {
        logic_oper = tokens->array[*iter];
        logic_oper->left_child = l_value;
        (*iter)++;

        logic_oper->right_child = GetPriority(tokens, iter);

        l_value = logic_oper;
    }

    return l_value;
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

static Node *GetPriority(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.str[0] == '(')
    {
        (*iter)++;
        free(tokens->array[*iter - 1]);

        Node *value = GetLogicOper(tokens, iter);

        if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.str[0] == ')')
        {
            (*iter)++;
            free(tokens->array[*iter - 1]);
            return value;
        }

        SyntaxError();
    }
    else
    {
        return GetArg(tokens, iter);
    }

    return nullptr;
}


static Node *GetArg(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (tokens->array[*iter]->node_type.bytes.is_number || tokens->array[*iter]->node_type.bytes.is_variable)
    {
        (*iter)++;

        return tokens->array[*iter - 1];
    }
    //else if (tokens->array[*iter]->node_type.bytes.is_func)
    //{
    //    Node *func = GetFunc(tokens, iter);
    //    return func;
    //}

    SyntaxError();
    return nullptr;
}

static int lexAnal(Tokens *tokens)
{
    assert(tokens);

    Suff_Tree *suff_tree = suffTreeCtor();

    int insd_rnd_brack = 0;
    int insd_fig_brack = 0;

    for (int i = 0; i < tokens->size; i++)
    {
        Node *token = tokens->array[i];

        insd_rnd_brack += (token->value.str[0] == '(');
        insd_rnd_brack -= (token->value.str[0] == ')');

        insd_fig_brack += (token->value.str[0] == '{');
        insd_fig_brack -= (token->value.str[0] == '}');

        if (token->node_type.bytes.is_variable)
        {
            int status = isKeyword(token->value.str, suff_tree);

            token->node_type.number = status + !status;

            if (!status && tokens->array[i + 1]->value.str[0] == '(')
            {
                token->node_type.number = 0;
                token->node_type.bytes.is_func = 1;
            }
        }
    }

    if (insd_fig_brack || insd_rnd_brack)
    {
        SyntaxError();
    }

    for (int i = 0; i < tokens->size; i++)
    {
        printf("str = %s, status = %d\n", tokens->array[i]->value.str, tokens->array[i]->node_type.number);
    }

    suffTreeDtor(suff_tree);
    return 0;
}
