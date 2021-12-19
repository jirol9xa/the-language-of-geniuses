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
static int tokensDtor(Tokens_t *tokens);
static int tokensResize(Tokens_t *tokens);
static Node *number();
static Node *identific();
static Node *oper();

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
static Node *GetGlobal(Tokens_t *tokens, int *iter, int is_const);
static Node *GetArgs(Tokens_t *tokens, int *iter, int for_global);
static int skipBrkts(Tokens_t *tokens, int *iter, int is_open, int is_round);
static Node *GetAddSub(Tokens_t *tokens, int *iter);
static Node *GetDefine(Tokens_t *tokens, int *iter);

#define INC(arg) ((*arg)++)

#define PRINT_RESHETKA printf("#######################################\n");

#define TYPE_BYTES(arg) tokens->array[*arg]->node_type.bytes

#define PRINT_NODE(arg)                         \
    {                                           \
        if (arg->node_type.bytes.is_number)     \
        {                                       \
            printf("%lg\n", arg->value.number); \
        }                                       \
        else                                    \
        {                                       \
            printf("%s\n", arg->value.str);     \
        }                                       \
    }

#define SYNTAX_ERR                 \
    {                              \
        PRINT_RESHETKA;            \
        printf("Syntax err in\n"); \
        PRINT_RESHETKA;            \
        PRINT_LINE;                \
        SyntaxError();             \
    }

#define FIRST_SYMB(iter) tokens->array[*iter]->value.str[0]

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

    tree->root = GetGeneral(&tokens);

    treeDump(tree);

    PRINT_LINE;
    //for (int i = 0; i < tokens.size; i++)
    //{
    //    PRINT_LINE;
    //    printf("tkn = %p\n", tokens.array[i]);
    //    if (!tokens.array[i]->node_type.bytes.is_number)
    //    {
    //        PRINT_LINE;
    //        printf("str = %s\n", tokens.array[i]->value.str);
    //        PRINT_LINE;
    //        free(tokens.array[i]->value.str);
    //    }
    //    PRINT_LINE;
    //    free(tokens.array[i]);
    //}
    tokensDtor(&tokens); // delete this!!!

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
        else if (*STRING >= 'a' && *STRING <= 'z' || *STRING >= 'A' && *STRING <= 'Z')
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

        lexAnal(tokens);

        Node **last_node = &(tokens->array[tokens->size]);

        (*last_node) = (Node *)calloc(1, sizeof(Node));
        (*last_node)->node_type.bytes.is_operator = 1;

        (*last_node)->value.str = (char *)calloc(2, sizeof(char));
        (*last_node)->value.str[0] = '$';
        (*last_node)->value.str[1] = '\0';

        PRINT_LINE;

        PRINT_NODE((*last_node));

        tokens->size++;

        return 0;
    }

    SYNTAX_ERR;
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

    free(tokens->array);
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

    sscanf(STRING, "%[a-zA-Z0-9_-]", buffer);

    printf("ident = %s\n", buffer);

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

    //SYNTAX_ERR;
    return nullptr;
}

static Node *oper()
{
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->value.str = (char *)calloc(3, sizeof(char));

    node->value.str[0] = *STRING++;
    node->value.str[1] = '\0';

    if (strchr("<>=", node->value.str[0]))
    {
        if (*STRING == '=')
        {
            node->value.str[1] = *STRING++;
            node->value.str[2] = '\0';
        }
    }

    PRINT_LINE;
    printf("value = %s\n", node->value.str);

    STRING += skipSpace(STRING);
    node->node_type.bytes.is_operator = 1;

    if (!strchr("+-/*^()$>=<{};,", node->value.str[0]))
    {
        PRINT_LINE;
        SYNTAX_ERR;
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

    Node *root = GetExternStatement(tokens, &iter); // change to statement I don't know what exactly

    if (tokens->array[iter]->node_type.bytes.is_operator && tokens->array[iter]->value.str[0] == '$')
    {
        iter++;
    }
    else
    {
        SYNTAX_ERR;
    }

    free(tokens->array[iter - 1]);
    return root;
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

    //todo while(*iter != tokens->size)

    Node *ext_stmnt = nullptr;
    Node *stmnt = nullptr;

    PRINT_LINE;
    while (!(tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.str[0] == '$'))
    {
        PRINT_LINE;
        PRINT_NODE(tokens->array[*iter]);

        stmnt = (Node *)calloc(1, sizeof(Node));
        PRINT_LINE;

        stmnt->node_type.bytes.is_serv_node = 1;

        stmnt->value.str = (char *)calloc(10, sizeof(char));
        strcpy(stmnt->value.str, "statement");
        PRINT_LINE;

        Node *node = tokens->array[*iter];

        PRINT_LINE;

        int is_const = (node->node_type.bytes.is_keyword == IS_CONST);

        PRINT_LINE;

        if (is_const || node->node_type.bytes.is_variable)
        {
            //INC(iter); skip const
            PRINT_LINE;
            Node *equal = GetGlobal(tokens, iter, is_const);

            INC(iter); // skipping ;

            stmnt->right_child = equal;
            equal->parent = stmnt;
        }
        else if (node->node_type.bytes.is_keyword = IS_DEFINE)
        {
            PRINT_LINE;
            PRINT_NODE(node);

            stmnt->right_child = GetDefine(tokens, iter);
            stmnt->right_child->parent = stmnt;

            PRINT_LINE;

            PRINT_NODE(tokens->array[*iter]);

            if (skipBrkts(tokens, iter, 0, 0))
            {
                SYNTAX_ERR;
            }

            PRINT_NODE(tokens->array[*iter]);
        }
        else
        {
            SYNTAX_ERR;
        }

        stmnt->left_child = ext_stmnt;
        ext_stmnt = stmnt;
    }

    return ext_stmnt;
}

/*!
    \brief Function for reading statement inside the function
*/
static Node *GetStatement(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *stmnt = readStatement(tokens, iter);
    Node *value = nullptr;

#define KEY_NUMBER tokens->array[*iter]->node_type.bytes.is_keyword

    while (tokens->array[*iter]->value.str[0] != '}' && (KEY_NUMBER != IS_WHILE) && (KEY_NUMBER != IS_IF))
    {
        PRINT_RESHETKA;
        PRINT_RESHETKA;
        PRINT_RESHETKA;

        PRINT_NODE(tokens->array[*iter]);

        value = readStatement(tokens, iter);

        value->left_child = stmnt;
        stmnt->parent = value;

        stmnt = value;
        //INC(iter);
    }

    return stmnt;
}

static Node *readStatement(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    skipBrkts(tokens, iter, 1, 0);

    //if (tokens->size = *iter + 1)

    //Node *node = tokens->array[*iter];
    Node *stmnt = (Node *)calloc(1, sizeof(Node));

    stmnt->node_type.bytes.is_serv_node = 1;

    stmnt->value.str = (char *)calloc(10, sizeof(char));
    strcpy(stmnt->value.str, "statement");

    Node *stmnt_val = nullptr;

    PRINT_NODE(tokens->array[*iter]);

    if (!TYPE_BYTES(iter).is_func && !TYPE_BYTES(iter).is_variable)
    {
        SYNTAX_ERR;
    }

    if (TYPE_BYTES(iter).is_func)
    {
        PRINT_LINE;
        PRINT_NODE(tokens->array[*iter]);
        PRINT_LINE;
        stmnt_val = GetFunc(tokens, iter);

        if (!TYPE_BYTES(iter).is_operator && tokens->array[*iter]->value.str[0] != ';')
        {
            SYNTAX_ERR;
        }

        INC(iter);
        return stmnt_val;
    }

    if (!TYPE_BYTES(iter + 1).is_operator && tokens->array[*iter + 1]->value.str[0] != '=')
    {
        SYNTAX_ERR;
    }

    stmnt_val = tokens->array[*iter + 1];
    stmnt_val->left_child = tokens->array[*iter];

    (*iter) += 2;

    PRINT_LINE;
    stmnt_val->right_child = GetAddSub(tokens, iter);

    //INC(iter);

    if (!TYPE_BYTES(iter).is_operator || tokens->array[*iter]->value.str[0] != ';')
    {
        SYNTAX_ERR;
    }

    INC(iter);

    stmnt->right_child = stmnt_val;
    stmnt_val->parent = stmnt;

    return stmnt;
}

/*!
    \brief Function for writing "while" in the Tree
*/
static Node *GetWhile(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *while_ = tokens->array[(*iter)++];

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

    Node *if_ = tokens->array[(*iter)++];

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
        SYNTAX_ERR;
    }

    Node *l_val = tokens->array[(*iter)++];
    if (!l_val->node_type.bytes.is_variable || !l_val->node_type.bytes.is_number)
    {
        SYNTAX_ERR;
    }

    Node *oper = tokens->array[(*iter)++];

    Node *r_val = tokens->array[(*iter)++];
    if (!r_val->node_type.bytes.is_variable || !r_val->node_type.bytes.is_number)
    {
        SYNTAX_ERR;
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
        INC(iter);

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

    Node *node = tokens->array[*iter];

    PRINT_NODE(tokens->array[*iter]);

    // mb SitaxError is too much

    if (node->node_type.bytes.is_number || node->node_type.bytes.is_variable)
    {
        INC(iter);

        return node;
    }
    if (is_func && node->node_type.bytes.is_func)
    {
        PRINT_LINE;

        return GetFunc(tokens, iter);
    }

    printf("Node = %p\n", node);

    return nullptr;
}

/*!
    \brief Function for getting function (generate call)
*/
static Node *GetFunc(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *call_node = (Node *)calloc(1, sizeof(Node));
    Node *func_node = (Node *)calloc(1, sizeof(Node));

    call_node->node_type.bytes.is_serv_node = 1;
    func_node->node_type.bytes.is_serv_node = 1;

    call_node->value.str = (char *)calloc(5, sizeof(char));
    func_node->value.str = (char *)calloc(5, sizeof(char));

    strcpy(call_node->value.str, "call");
    strcpy(func_node->value.str, "func");

    func_node->left_child = tokens->array[(*iter)++];
    func_node->left_child->parent = func_node;

    PRINT_NODE(tokens->array[*iter]);

    func_node->right_child = GetArgs(tokens, iter, 0);
    func_node->right_child->parent = func_node;

    call_node->right_child = func_node;
    func_node->parent      = call_node;

    return call_node;
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
            SYNTAX_ERR;
        }

        return node;
    }

    if (node->node_type.bytes.is_operator)
    {
        SYNTAX_ERR;
    }

    return node;
}

/*!
    \brief Function for skipping brackets
*/
static int skipBrkts(Tokens_t *tokens, int *iter, int is_open, int is_round)
{
    assert(tokens);
    assert(iter);

    int is_skipped = 0;

    if (is_open)
    {
        if (is_round)
        {
            while (TYPE_BYTES(iter).is_operator && strlen(tokens->array[*iter]->value.str) == 1 && FIRST_SYMB(iter) == '(')
            {
                is_skipped++;

                free(tokens->array[*iter]); //mb segfold
                INC(iter);
            }
        }
        else
        {
            while (TYPE_BYTES(iter).is_operator && strlen(tokens->array[*iter]->value.str) == 1 && FIRST_SYMB(iter) == '{')
            {
                is_skipped++;

                free(tokens->array[*iter]); //mb segfold
                INC(iter);
            }
        }
    }

    if (!is_open)
    {
        if (is_round)
        {
            while (TYPE_BYTES(iter).is_operator && strlen(tokens->array[*iter]->value.str) == 1 && FIRST_SYMB(iter) == ')')
            {
                is_skipped++;

                free(tokens->array[*iter]); //mb segfold
                INC(iter);
            }
        }
        else
        {
            while (TYPE_BYTES(iter).is_operator && strlen(tokens->array[*iter]->value.str) == 1 && FIRST_SYMB(iter) == '}')
            {
                PRINT_LINE;
                is_skipped++;

                free(tokens->array[*iter]); //mb segfold
                INC(iter);
                PRINT_LINE;
                PRINT_NODE(tokens->array[*iter]);
                PRINT_LINE;
            }
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
        INC(iter);

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
        INC(iter);
        free(tokens->array[*iter - 1]);

        Node *value = GetAddSub(tokens, iter);

        if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.str[0] == ')')
        {
            INC(iter);
            free(tokens->array[*iter - 1]);
            return value;
        }

        SYNTAX_ERR;
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
static Node *GetArgs(Tokens_t *tokens, int *iter, int for_global)
{
    assert(tokens);
    assert(iter);

    PRINT_LINE;

    if (!for_global && skipBrkts(tokens, iter, 1, 1))
    {
        printf("Node = ");
        PRINT_NODE(tokens->array[*iter]);
        SYNTAX_ERR;
    }

    PRINT_NODE(tokens->array[*iter]);

    // checking for a function without arguments
    if (!skipBrkts(tokens, iter, 0, 1))
    {
        return nullptr;
    }

    Node *args = GetAddSub(tokens, iter);
    Node *param_node = (Node *)calloc(1, sizeof(Node));

    param_node->node_type.bytes.is_serv_node = 1;

    param_node->value.str = (char *)calloc(6, sizeof(char));
    strcpy(param_node->value.str, "param");

    param_node->right_child = args;
    args->parent = param_node;

    while ((!TYPE_BYTES(iter).is_operator || FIRST_SYMB(iter) != ')') && FIRST_SYMB(iter) != ';')
    {
        if (!TYPE_BYTES(iter).is_number && FIRST_SYMB(iter) == ',')
        {
            free(tokens->array[*iter]->value.str);
            free(tokens->array[*iter]);

            INC(iter);
        }
        PRINT_LINE;

        Node *next_arg = GetAddSub(tokens, iter);

        if (!next_arg)
        {
            //next_arg = tokens->array[*iter];
            //INC(iter);
            return param_node;
            ;
        }

        Node *new_param_node = (Node *)calloc(1, sizeof(Node));

        new_param_node->node_type.bytes.is_serv_node = 1;

        new_param_node->value.str = (char *)calloc(6, sizeof(char));
        strcpy(new_param_node->value.str, "param");

        new_param_node->right_child = next_arg;
        next_arg->parent = new_param_node;

        new_param_node->left_child = param_node;
        param_node->parent = new_param_node;

        param_node = new_param_node;

        if (TYPE_BYTES(iter).is_operator && tokens->array[*iter]->value.str[0] == ',')
        {
            free(tokens->array[*iter]->value.str);
            free(tokens->array[*iter]);

            INC(iter);
        }
    }

    if (TYPE_BYTES(iter).is_operator && FIRST_SYMB(iter) == ')')
    {
        INC(iter);
    }

    return param_node;
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
static Node *GetDefine(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *def_node = tokens->array[(*iter)++];
    Node *func_node = (Node *)calloc(1, sizeof(Node));

    def_node->node_type.bytes.is_serv_node = 1;

    //strcpy(def_node->value.str, "define");

    func_node->node_type.bytes.is_serv_node = 1;
    func_node->node_type.bytes.is_keyword = IS_FUNCTION;

    func_node->value.str = (char *)calloc(5, sizeof(char));
    strcpy(func_node->value.str, "func");

    PRINT_NODE(tokens->array[*iter]);
    func_node->left_child = tokens->array[(*iter)++]; // skip func name
    PRINT_LINE;
    PRINT_NODE(tokens->array[*iter]);

    func_node->right_child = GetArgs(tokens, iter, 0);

    def_node->left_child = func_node;
    def_node->right_child = GetStatement(tokens, iter);

    PRINT_LINE;

    return def_node;
}

static Node *GetAddSub(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *oper = nullptr;
    Node *value = GetMulDiv(tokens, iter);

    while (tokens->array[*iter]->node_type.bytes.is_operator && strchr("+-<>=", tokens->array[*iter]->value.str[0]))
    {
        oper = tokens->array[*iter];
        oper->left_child = value;
        INC(iter);

        oper->right_child = GetMulDiv(tokens, iter);

        value = oper;
    }

    return value;
}

static Node *GetGlobal(Tokens_t *tokens, int *iter, int is_const)
{
    assert(tokens);
    assert(iter);

    (*iter) += is_const;

    Node *name_node = tokens->array[(*iter)++];

    Node *equal = tokens->array[(*iter)++];

    if (!equal->node_type.bytes.is_operator || equal->value.str[0] != '=')
    {
        SYNTAX_ERR;
    }

    equal->left_child = name_node;
    name_node->parent = equal;

    equal->right_child = GetArgs(tokens, iter, 1); //returned with param_node
    

    if (!equal->right_child)
    {
        SYNTAX_ERR;
    }
    
    equal->right_child = equal->right_child->right_child;
    
    free(equal->right_child->parent);

    equal->right_child->parent = equal;

    if (!tokens->array[*iter]->node_type.bytes.is_operator || tokens->array[*iter]->value.str[0] != ';' || !equal->right_child)
    {
        SYNTAX_ERR;
    }

    if (is_const)
    {
        Node *const_node = (Node *)calloc(1, sizeof(Node));

        const_node->node_type.bytes.is_keyword = IS_CONST;

        const_node->value.str = (char *)calloc(6, sizeof(char));
        strcpy(const_node->value.str, "const");

        name_node->left_child = const_node;
        const_node->parent = name_node;

        return equal;
    }

    return equal;
}

static int skipEndl(Tokens_t *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *node = tokens->array[*iter];

    if (node->node_type.bytes.is_operator && node->value.str[0] == ';')
    {
        INC(iter);

        return 0;
    }

    return -1;
}
