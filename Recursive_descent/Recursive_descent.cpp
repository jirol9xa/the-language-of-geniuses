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
#include "Lexer.h"


static int constructTokens(Tokens_t *tokens, char *string);
static int tokensCtor(Tokens_t *tokens);
static Node *number();
static Node *identific();
static Node *oper();
static int tokensResize(Tokens_t *tokens);

static Node *GetGeneral(Tokens_t *tokens);
static Node *GetArg(Tokens_t *tokens, int *iter);
static Node *GetPriority(Tokens_t *tokens, int *iter);
static Node *GetLRValue(Tokens_t *tokens, int *iter, int is_left); // for reading left or right value
static Node *GetFunc(Tokens_t *tokens, int *iter);
static Node *GetNumVar(Tokens_t *tokens, int *iter, int is_func);
static Node *GetLogicOper(Tokens_t *tokens, int *iter);
static Node *GetOperator(Tokens_t *tokens, int *iter);
static Node *GetIf(Tokens_t *tokens, int *iter);
static Node *GetWhile(Tokens_t *tokens, int *iter);
static Node *readStatement(Tokens_t *tokens, int *iter); //for reading statement
static Node *GetStatement(Tokens_t *tokens, int *iter);
static Node *GetExternStatement(Tokens_t *tokens, int *iter);
static int skipBrkts(Tokens_t *tokens, int *iter, int is_open);


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

    Tokens_t tokens = {};
    tokensCtor(&tokens);

    constructTokens(&tokens, string);

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
    \param  [Tokens_t *]tokens Pointer to tokens struct
    \param  [char *]string Array with code
    \return 0 if all right
*/
static int constructTokens(Tokens_t *tokens, char *string)
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


static int tokensCtor(Tokens_t *tokens)
{
    assert(tokens);

    tokens->array = (Node **)calloc(1, sizeof(Node *));
    tokens->capacity = 1;

    return 0;
}


static int tokensDtor(Tokens_t *tokens)
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


int SyntaxError()
{
    printf("!!! ERROR SyntaxError !!!\n");
    exit(1);
}


static Node *GetGeneral(Tokens_t *tokens)
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
    \param  [Tokens_t *]tokens Pointer to tokens
    \param  [int *]iter Pointer to current token
    \return [Node *] Pointer to new sub tree
*/
static Node *GetExternStatement(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (*iter = tokens->size)
    {
        return nullptr;
    }

    Node *node = tokens->array[*iter];

    if (node->node_type.bytes.is_keyword & IS_CONST || node->node_type.bytes.is_variable)
    {
        Node *equal = (Node *)calloc(1, sizeof(Node));
        Node *type = (Node *)calloc(1, sizeof(Node));

        Node *name =
    }

    //Node *stmnt = (Node *) calloc(1, sizeof(Node));
}

/*!
    \brief Function for reading statement inside the function
*/
static Node *GetStatement(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *stmnt = nullptr;
    Node *value = nullptr;

#define KEY_NUMBER tokens->array[*iter]->node_type.bytes.is_keyword

    while (tokens->array[*iter]->value.str[0] != '}' && (!KEY_NUMBER & IS_WHILE) && (!KEY_NUMBER & IS_IF))
    {
        value = readStatement(tokens, iter);

        *iter++;
    }
}


static Node *readStatement(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    #define IS_NUM(arg)     tokens->array[arg]->node_type.bytes.is_number
    #define IS_VAR(arg)     tokens->array[arg]->node_type.bytes.is_variable
    #define TYPE_BYTES(arg) tokens->array[arg]->node_type.bytes

    //if (tokens->size = *iter + 1)

    //Node *node = tokens->array[*iter];

    if (!TYPE_BYTES(*iter).is_func && !TYPE_BYTES(*iter).is_variable)
    {
        SyntaxError();
    }

    if (TYPE_BYTES(*iter).is_func)
    {
        
    }

    int stmnt_type = 0;
    int i = 0;

    if ()

    //while (TYPE_BYTES(*iter).is_number || (tokens->array[*iter]->value.str[0] != ';')) //(IS_NUM(*iter + i) || IS_VAR(*iter + i))
    //{
    //    if (IS_NUM(*iter + 1) && IS_NUM(*iter + i + 1) || IS_VAR(*iter + i) && IS_VAR(*iter + i + 1))
    //    {
    //        SyntaxError();
    //    }
//
    //    i++;
    //}
//
    //if (TYPE_BYTES(*iter + i).is_keyword)
    //{
    //    // add creating if, while
    //}
    //else if (TYPE_BYTES(*iter + i).is_operator)
    //{
    //    // add creating operations
    //}
    //else if (TYPE_BYTES(*iter + i).is_func)
    //{
    //    // add creating functions
    //}
    //else               // TODO:
    //{                  // Delete
    //    PRINT_LINE;    // this
    //    SyntaxError(); // ssssshhhhhh
    //}                  // it
//
#undef IS_NUM
#undef IS_VAR
#undef TYPE_BYTES
}


/*!
    \brief Function for writing "while" in the Tree
*/
static Node *GetWhile(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *while_ = tokens->array[*iter++];

    while_->left_child = GetOperator(tokens, iter);
    while_->right_child = GetStatement(tokens, iter); // mb bulshit

    return while_;
}


/*!
    \brief Function for writing "If" in the tree
*/
static Node *GetIf(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *if_ = tokens->array[*iter++];

    if_->left_child = GetOperator(tokens, iter);
}


// Use GetArg or GerLRValue
// func is too big
/*!
    \brief  Function that building sub tree for (<>=+*-/)
    \param  [Tokens *]tokens Pointer to tokens
    \param  [int *]iter Position of current token
    \return Pointer on sub tree 
*/
static Node *GetOperator(Tokens_t *tokens, int *iter)
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

    Node *oper = tokens->array[*iter++];

    Node *r_val = tokens->array[*iter++];
    if (!r_val->node_type.bytes.is_variable || !r_val->node_type.bytes.is_number)
    {
        SyntaxError();
    }

    oper->left_child = l_val;
    oper->right_child = r_val;

    return oper;
}


/*!
    \brief Fucntion for complex logical operations
*/
static Node *GetLogicOper(Tokens_t *tokens, int *iter)
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


/*!
    \brief Function for geetting number or variable
*/
static Node *GetNumVar(Tokens_t *tokens, int *iter, int is_func)
{
    assert(tokens);
    assert(iter);

    Node *node = nullptr;

    // mb SitaxError is too much

    if (node->node_type.bytes.is_number || node->node_type.bytes.is_variable)
    {
        node = tokens->array[*iter++];
    }
    if (is_func && node->node_type.bytes.is_func)
    {
        node = tokens->array[*iter++];
    }

    return node;
}


/*!
    \brief Function for getting function (generate call)
*/
static Node *GetFunc(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *func = tokens->array[*iter];
    Node *args = nullptr;

    if (!func->node_type.bytes.is_func)
    {
        return nullptr;
    }

    (*iter)++;

    if (strstr("sincosln", func->value.str))
    {
        func->right_child = GetNumVar(tokens, iter, 0);

        return func;
    }

    args = GetArg(tokens, iter);

    func->right_child = args;

    return func;
}


/*!
    \brief Fucntion for getting LRValue
*/
static Node *GetLRValue(Tokens_t *tokens, int *iter, int is_left) // for reading left or right value
{
    assert(tokens);
    assert(iter);

    Node *node = tokens->array[*iter];

    if (is_left)
    {
        if (!node->node_type.bytes.is_variable)
        {
            SyntaxError();
        }

        return node;
    }

    if (node->node_type.bytes.is_operator)
    {
        SyntaxError();
    }

    return node;
}


/*!
    \brief Function for skipping brackets
*/
static int skipBrkts(Tokens_t *tokens, int *iter, int is_open)
{
    assert(tokens);
    assert(iter);

    int is_skipped = 0;

    if (is_open)
    {
        while (strlen(tokens->array[*iter]->value.str) == 1 && strchr("(", tokens->array[*iter]->value.str[0]))
        {
            is_skipped++;

            free(tokens->array[*iter]); //mb segfold
            (*iter)++;
        }
    }

    if (!is_open)
    {
        while (strlen(tokens->array[*iter]->value.str) == 1 && strchr(")", tokens->array[*iter]->value.str[0]))
        {
            is_skipped++;

            free(tokens->array[*iter]); //mb segfold
            (*iter)++;
        }
    }

    return !is_skipped;
}


static Node *GetMulDiv(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *oper = nullptr;

    Node *value = GetPriority(tokens, iter);

    while (tokens->array[*iter]->node_type.bytes.is_operator && strchr("*^/&|", tokens->array[*iter]->value.str[0]))
    {
        oper = tokens->array[*iter];
        oper->left_child = value;
        (*iter)++;

        oper->right_child = GetPriority(tokens, iter);

        value = oper;
    }

    return value;
}


/*!
    \brief Fucntion for getting ()
*/
static Node *GetPriority(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.str[0] == '(')
    {
        (*iter)++;
        free(tokens->array[*iter - 1]);

        Node *value = GetAddSub(tokens, iter);

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
        return GetNumVar(tokens, iter, 1);
    }

    return nullptr;
}


/*!
    \brief Function for getting args in func
*/
static Node *GetArgs(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (!skipBrkts(tokens, iter, 1))
    {
        SyntaxError();
    }

    Node *args = GetAddSub(tokens, iter);
    Node *next_arg = nullptr;
    Node *param_node = nullptr;

    while (tokens->array[*iter]->value.str[0] != ')')
    {
        next_arg = GetAddSub(tokens, iter);

        if (!next_arg)
        {
            //next_arg = tokens->array[*iter];
            //*iter++;
            return args;
        }

        param_node = (Node *)calloc(1, sizeof(Node));

        param_node->parent = args;
        args->left_child = param_node;

        param_node->right_child = next_arg;
        next_arg->parent = param_node;

        if (tokens->array[*iter]->value.str[0] == ',')
        {
            *iter++;
        }

        args = param_node;
    }

    *iter++;

    return args;
    //return tokens->array[*iter - 1];

    //else if (tokens->array[*iter]->node_type.bytes.is_func)
    //{
    //    Node *func = GetFunc(tokens, iter);
    //    return func;
    //}
}


static Node *GetBlock(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    static int in_block = 0;

    Node *node = GetStatement(tokens, iter);

    do
    {
        int is_open = tokens->array[*iter]->value.str[0] == '{';
        // node = call function
        in_block += is_open + (-1) * !is_open;
    } while (!in_block);

    return node;
}


/*!
    \brief Function for getting definition of func
*/
static Node *GetDefine(Tokens_t tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *def_node = (Node *)calloc(1, sizeof(Node));
    Node *func_node = (Node *)calloc(1, sizeof(Node));

    def_node->node_type.bytes.is_serv_node = 1;
    def_node->node_type.bytes.is_keyword = IS_DEFINE;

    //strcpy(def_node->value.str, "define");

    func_node->node_type.bytes.is_serv_node = 1;
    func_node->node_type.bytes.is_keyword = IS_FUNCTION;

    func_node->left_child = tokens.array[*iter++];
    func_node->right_child = GetArgs(tokens, iter);

    def_node->left_child = func_node;
    def_node->right_child = GetStatement(tokens, iter);

    return def_node;
}


static Node *GetAddSub(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *oper = nullptr;
    Node *value = GetMulDiv(tokens, iter);

    while (tokens->array[*iter]->node_type.bytes.is_operator && strchr("+-<>=", tokens->array[*iter]->value.symbol))
    {
        oper = tokens->array[*iter];
        oper->left_child = value;
        (*iter)++;

        oper->right_child = GetMulDiv(tokens, iter);

        value = oper;
    }

    return value;
}