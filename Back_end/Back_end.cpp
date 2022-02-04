//A.p Проверяй на C++
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Tree.h"
#include "Back_end.h"
#include "Stack.h"
#include "Logger.h"
#include "Syntax.h"


static Name *nameCtor();
static int   generateGeneral (Tree  *tree,  int   *iter);
static int   generateStmnt   (Node  *stmnt, Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateDefine  (Node  *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateGlobal  (Node  *node,  Glob_Name_space *glob_name_space, Stack *name_space,   int is_init);
static int   generateVar     (Node  *node,  Glob_Name_space *glob_name_space, Stack *name_space,   int is_init);
static int   addNewName      (Node  *node,  Glob_Name_space *glob_name_space, Stack *name_space, int  is_var, int *shift);
static int   generateCall    (Node  *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateArgs    (Node  *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateFuncCode(Node  *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateMath    (Node  *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   makeArgs        (Node *node,   Glob_Name_space *name_space);
static int   writeNode       (Node  *node,  int   *iter);

static int   getIndex        (Stack *name_space, char *name, int is_func);
static int  findElem         (Stack *name_space, char *elem);


#define KWORD_TYPE(arg) arg->node_type.bytes.is_keyword

#define INC_VAR_AMNT           \
{                              \
    writeLogs("PUSH [ax]");    \
    writeLogs("PUSH 1");       \
    writeLogs("ADD");          \
    writeLogs("POP [ax]");     \
}

#define SYNTAX_ERR             \
{                              \
    PRINT_RESHETKA;            \
    printf("Syntax err in\n"); \
    PRINT_RESHETKA;            \
    PRINT_LINE;                \
    SyntaxError();             \
}

#define INC(ptr, num) ((*ptr)+= num)


static Name *nameCtor()
{
    return (Name *) calloc(1, sizeof(Name));
}


int generateAsm(Tree *tree, const char *dst_name)
{
    assert(dst_name);
    assert(tree);
    IS_DTRCT(tree);

    openLogs(dst_name);

    int iter = 0;

    Stack name_space = {};
    stackCtor(&name_space, 0);

    Glob_Name_space global_name_space = {};
    stackCtor(&(global_name_space.func), 0);
    stackCtor(&(global_name_space.global_vars), 0);

    // creating const memory
    writeLogs("PUSH 20\n");
    writeLogs("POP bx\n");
    // bx contains the beginning of a free memory 

    writeLogs("JMP :main\n");

    generateStmnt(tree->root, &global_name_space, &name_space, &iter); 

    stackDtor(&(global_name_space.func));
    stackDtor(&(global_name_space.global_vars));
    closeLogs();

    return 0;
}


static int generateGeneral(Tree *tree, int *iter)
{
    assert(tree);
    assert(iter);
    IS_DTRCT(tree);

    writeLogs("JMP :main\n");

    //generateStmnt(tree->root,)

    return 0;
}


static int generateStmnt(Node *stmnt, Glob_Name_space *glob_name_space, Stack *name_space, int *iter)
{
    assert(stmnt);
    assert(glob_name_space);
    assert(iter);

    if (stmnt->left_child)
    {   
        generateStmnt(stmnt->left_child, glob_name_space, name_space, iter);
    }

    Node *node = stmnt->right_child;
    auto node_type = node->node_type.bytes;

    if (node_type.is_operator && node->value.str[0] == '=')
    {
        Node *left     = node->left_child;
        auto left_type = left->node_type.bytes;

        if (!left_type.is_variable)
        {
            SYNTAX_ERR;
        }

        generateMath(node->right_child, glob_name_space,name_space, iter);

        // adding var in name_space
        // !!!!make function!!!!
        Name *new_var = nameCtor(); 

        strcpy(new_var->name, left->value.str);
        //strcpy(new_var->func_name, "GLOBAL");
        new_var->Ntype.is_var = 1;
        //
        
        int shift = -1;
        if (name_space)
        {
            shift      = findElem(name_space, left->value.str);
        }

        int glob_shift = findElem(&(glob_name_space->global_vars), left->value.str);

        if (name_space && shift == -1) //for local vars
        {
            if (shift == -1) //if doesn't exist
            {
                stackPush(name_space, new_var);

                writeLogs("POP [bx + ax]\n");
                INC_VAR_AMNT;
            }
            else //if exist
            {
                writeLogs("POP [bx + %d]\n", shift);
            }
        }
        else if (glob_shift == -1) //for glob vars, if it's not exists
        {
            stackPush(&(glob_name_space->global_vars), new_var);
            
            writeLogs("POP [bx + ax]\n");
            INC_VAR_AMNT;
        }
        else // if glob var exists
        {
            writeLogs("POP [%d]\n", glob_shift);
        }
    }
    else if (node_type.is_keyword == IS_DEFINE)
    {
        generateDefine(node, glob_name_space,name_space, iter);
    }
    else if (node_type.is_keyword == IS_RETURN)
    {
        generateMath(node->right_child, glob_name_space,name_space, iter);
        
        writeLogs("POP cx\n");
        writeLogs(":RET\n");
    }
    else if (node_type.is_keyword == IS_CALL)
    {
        generateCall(node, glob_name_space, name_space);
    }

    return 0;
}


static int generateGlobal(Node *node, Glob_Name_space *glob_name_space, Stack *name_space, int *iter, int is_init)
{
    assert(node);
    assert(name_space);
    assert(iter);

    Name *glob = findElem(name_space, node->value.str);

    if (glob)
    {

    }

    return 0;
}


//static int generateVar(Node *node, List *name_space, int *iter, int is_init)
//{
//    assert(node);
//    assert(name_space);
//    assert(iter);
//
//    Name *name = findElem(name_space, node->value.str);
//
//    if (!is_init)
//    {
//        if (!name->type.is_var)
//        {
//            printf("!!! ERROR Can't perform assignment to a function !!!\n");
//            SYNTAX_ERR;
//        }
//
//        writeLogs("PUSH [%d]", name->index.ram_indx);
//        
//        return 0;
//    }
//
//    writeLogs("POP [%d]\n", name->index.ram_indx);
//    
//    return 0;
//}


static int writeNode(Node *node, int *iter)
{
    assert(node);
    assert(iter);

    return 0;
}


//static int addNewName(Node *node, List *name_space, int is_var, int *shift)
//{
//    assert(node);
//    assert(name_space);
//
//    Name *var           =  nameCtor();
//    var->type.is_var    =  is_var;
//    var->type.is_func   =  !is_var;
//    var->name           =  node->value.str;
//    var->index.ram_indx = *shift;
//
//    listPushBack(name_space, var);
//
//    return 0;
//}


static int generateDefine(Node *node, Glob_Name_space *glob_name_space, Stack *name_space, int *shift)
{    
    assert(node);
    if (KWORD_TYPE(node) != IS_DEFINE)
    {
        printf("!!! ERROR Node if generateDefine doesn't have define type !!!\n");
        return -1;
    }
    if (name_space)
    {
        printf("!!! ERROR Can't define a function inside another !!!\n");
    }

    Stack Name_space = {};
    stackCtor(&Name_space, 0);

    Node *func_node = node->left_child;

    // хорошая идея, но пока не надо
    addNewFunc(func_node->left_child, glob_name_space, 0);

    if (func_node->right_child)
    {
        makeArgs(func_node->right_child, name_space);
    }

    writeLogs(":%s\n", func_node->left_child->value.str);

    if (node->right_child)
    {
        generateStmnt(node->right_child, glob_name_space, &Name_space, shift);
    }

    auto func_type = func_node->left_child->node_type.bytes;

    //writeLogs("POP [bx + ]")
    
    if (func_type.is_keyword == IS_MAIN)
    {
        writeLogs("HLT\n");
    } 

    stackDtor(&Name_space);
    return 0;  
}

        ////////////////////////////////////////////////////////////////////////
        // Кароче bx хранит адрес первой доступной ячейки памяти в оперативке //
        // [bx - 1] хранит адрес прошлого значения ax                         //
        // В ax хранится количество занятых ячеек памяти в данной функции     //
        // относительно bx                                                    //
        // Каждой локальной переменной соответсвует адрес отностительно bx    //
        // Если переменная глобальная, то у нее абсолютный адрес              //
        ////////////////////////////////////////////////////////////////////////

static int generateCall(Node *node, Glob_Name_space *glob_name_space, Stack *name_space)
{
    assert(node);
    assert(name_space);

    Node *func_node = node->right_child;

    if (!findElem(name_space, func_node->left_child->value.str))
    {
        printf("!!! ERROR Can't find func with name %s !!!\n", func_node->left_child->value.str);
        SYNTAX_ERR;
    }

    Node *args = func_node->right_child;

    // подготовим почву для изменения bx
    writeLogs("PUSH bx");
    writeLogs("POP [bx + ax]");
    INC_VAR_AMNT;

    // изменим значение bx
    writeLogs("PUSH ax");
    writeLogs("PUSH bx");
    writeLogs("ADD");
    writeLogs("POP bx");

    // занулим ax
    writeLogs("PUSH 0");
    writeLogs("POP ax");

    if (args)
    {
        generateArgs(args, glob_name_space, name_space);
    }

    writeLogs(":CALL %s\n", func_node->left_child->value.str);
    writeLogs("PUSH cx");

    // выставляем нужное значение ax
    writeLogs("PUSH [bx - 1]");
    writeLogs("POP   ax");

    // изменяем значение bx на предыдущее
    writeLogs("PUSH bx");
    writeLogs("PUSH ax");
    writeLogs("SUB");
    writeLogs("POP  bx");

    return 0;
}


/*!
    \brief A function for generating arguments to a function
    \param [Node *]node Poiter to first param node
    \param [int *]iter Number of cmd
*/
static int generateArgs(Node *node, Glob_Name_space *glob_name_space, Stack *name_space, int *iter)
{
    assert(node);
    assert(name_space);

    int args_amnt = 0;

    do
    {
        Node *param      = node->right_child;
        auto  param_type = param->node_type.bytes;

        if (param_type.is_number)
        {
            writeLogs("PUSH %d\n", param->value.number);
        }
        else if (param_type.is_variable)
        {
            //различать локальные и глобальные переменные т.к. локальные 
            //завязаны на bx
            char *name = param->value.str;

            int index = findElem(name_space, name);
            if (index >= 0)
            {
                writeLogs("PUSH [bx + %d]\n", index);
            }
            else
            {
                index = findElem(glob_name_space, name);
            }

            if (index >= 0)
            {
                writeLogs("PUSH [%d]\n", index);
            }
            else
            {
                printf("!!! ERROR Can't find variable \"%s\" !!!\n", name);
                SYNTAX_ERR;
            }
        }

        // пока что хотя бы числа и аргументы передавать
        //else if (param_type.is_keyword == IS_CALL)
        //{
        //    generateCall(param, name_space, iter);
        //    //int index = getIndex(name_space, param->value.str, 1);
        //}

        writeLogs("POP [bx + ax]\n"); // вычислять смещение
        INC_VAR_AMNT;
    }
    while (node->left_child);

    return 0;
}


static int getIndex(Glob_Name_space *global_name_space, Stack *name_space, char *name)
{
    assert(name_space);
    assert(name);

    int index = findElem(name_space, name);

    if (index == -1)
    {
        index = findElem(global_name_space, name);
    }

    if (index == 0 || index == -1)
    {
        printf("!!! ERROR Can't find variable \"%s\" !!!\n", name);

        return -1;
    }

    return index;
}


// mb useless
static int generateFuncCode(Node *node, Glob_Name_space *glob_name_space, Stack *name_space)
{
    assert(node);
    assert(name_space);

    int start_size = name_space->size;

    return 0;
}


/*!
    \brief  Function for making math expressions and putting 
            result in stack
*/
static int generateMath(Node *node, Glob_Name_space *glob_name_space, Stack *name_space, int *iter)
{
    assert(node);
    assert(name_space);
    assert(iter);

    bool is_call = (node->node_type.bytes.is_keyword == IS_CALL);

    
    if (node->left_child && !is_call)
    {
        generateMath(node->left_child, glob_name_space, name_space, iter);
    }
    if (node->right_child && !is_call)
    {
        generateMath(node->right_child, glob_name_space, name_space, iter);
    }

    auto node_type = node->node_type.bytes;

    if (node_type.is_number)
    {
        //printf("value = %lg ptr = %p\n", node->value.number, node);
        writeLogs("PUSH %lg\n", node->value.number);
    }
    else if (node_type.is_keyword == IS_CALL)
    {
        generateCall(node, name_space, iter);
    }
    else if (node_type.is_operator)
    {
        //printf("node value = %s\n", node->value.str);

        switch (node->value.str[0])
        {
            case '+':
                writeLogs("ADD\n");
                break;
            case '-':
                writeLogs("SUB\n");
                break;
            case '*':
                writeLogs("MUL\n");
                break;
            case '/':
                
                writeLogs("DIV\n");
                break;
        } 
    }
    else if (node_type.is_variable)
    {

    }

    return 0;
}


/*!
    \brief  Function for searching var or func in name_space
    \param [Stack *]name_space Name space
    \param [char *]elem Name of searching elem
    \param [char *]func_name Name of fun that addresses element
    \return returns rem idx if var, -2 if func and -1 if 
    search was failed 
*/
static int findElem(Stack *name_space, char *elem)
{
    assert(name_space);
    assert(elem);

    for (int i = name_space->size ; i >= 0; i--)
    {
        Name* name_member = name_space->data[i];

        if (!strcmp(name_member->name, elem))
        {
            return (name_member->Ntype.is_func) * (-2) + name_member->Ntype.is_var * i;
        }
    }

    return -1;
}


/*!
    \brief  Function for adding func parameters to 
            local name_space
    \param  [Node *]node First param node
    \param  [Stack *]name_space
*/
static int makeArgs(Node *node, Stack *name_space)
{
    assert(node);
    assert(name_space);

    do 
    {   
        Node *right = node->right_child;

        Name *new_arg = nameCtor();
        strcpy(new_arg->name, right->value.str);
        new_arg->Ntype.is_var = 1;

        stackPush(name_space, new_arg);
    }
    while (node->left_child);

    return 0;
}


static int addNewFunc(Glob_Name_space *global_name_space, char *name)
{
    assert(global_name_space);
    assert(name);

    if (findElem(&(global_name_space->func), name) != -1 || findElem(&(global_name_space->global_vars), name) != -1)
    {
        printf("!!! ERROR Func or var with name \"%s\" already exist !!!\n", name);
        SYNTAX_ERR;
    }

    Name *new_func = nameCtor();

    strcpy(new_func->name, name);
    new_func->Ntype.is_func = 1;

    stackPush(&(global_name_space->func), new_func);

    return 0;
}