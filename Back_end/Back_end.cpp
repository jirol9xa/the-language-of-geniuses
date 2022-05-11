//A.p Проверяй на C++
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Tree.h"
#include "Stack.h"
#include "Back_end.h"
#include "Logger.h"
#include "Syntax.h"


static Name *nameCtor();
static int   nameDtor(Name *name);
static int   showNames(Stack *name_space);


static int   generateGeneral (Tree *tree,  int   *iter);
static int   generateStmnt   (Node *stmnt, Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateDefine  (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateGlobal  (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space,   int is_init);
static int   generateVar     (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space,   int is_init);
static int   generateCall    (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateArgs    (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateFuncCode(Node *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateMath    (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateIf      (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generatePrintf  (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   generateScanf   (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space);
static int   addNewName      (Node *node,  Glob_Name_space *glob_name_space, Stack *name_space, int  is_var, int *shift);
static int   makeArgs        (Node *node,  Stack *name_space);
static int   addNewFunc      (Glob_Name_space *global_name_space, char *name);

static int   getIndex        (Stack *name_space, char *name, int is_func);
static int   findElem        (Stack *name_space, char *elem);


#define KWORD_TYPE(arg) arg->node_type.bytes.is_keyword

#define INC_VAR_AMNT           \
{                              \
    writeLogs("PUSH ax\n");    \
    writeLogs("PUSH 1\n");     \
    writeLogs("ADD\n");        \
    writeLogs("POP ax\n");     \
}

#define INC_DX                 \
{                              \
    writeLogs("PUSH dx\n");    \
    writeLogs("PUSH 1\n");     \
    writeLogs("ADD\n");        \
    writeLogs("POP dx\n");     \
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

#define OCCUPIED(name)                                       \
{                                                            \
    printf("!!! ERROR Name %s already exist !!!\n", name);   \
    SYNTAX_ERR;                                              \
}

#define CHECK_NAME(glob_name_space, name, is_func)                  \
{                                                                   \
    if (is_func)                                                    \
    {                                                               \
        if (findElem(&(glob_name_space->func), name) != -1)         \
        {                                                           \
            OCCUPIED(name);                                         \
        }                                                           \
        if (findElem(&(glob_name_space->global_vars), name) != -1)  \
        {                                                           \
            OCCUPIED(name);                                         \
        }                                                           \
    }                                                               \
    else                                                            \
    {                                                               \
        if (findElem(&(glob_name_space->func), name) != -1)         \
        {                                                           \
            OCCUPIED(name);                                         \
        }                                                           \
    }                                                               \
}

#define SHOW_NAMES(name_space)    \
{                                 \
    PRINT_LINE;                   \
    showNames(name_space);        \
    PRINT_LINE;                   \
}


//int IF_AMNT = 0;

static Name *nameCtor()
{
    Name *name = (Name *) calloc(1, sizeof(Name));
    name->name = (char *) calloc(32, sizeof(char));
    
    return name;
}


int generateAsm(Tree *tree, const char *dst_name)
{
    assert(dst_name);
    assert(tree);
    IS_DTRCT(tree);

    openLogs(dst_name);

    int iter = 0;

    Glob_Name_space global_name_space = {};
    stackCtor(&(global_name_space.func), 0);
    stackCtor(&(global_name_space.global_vars), 0);

    // creating const memory
    writeLogs("PUSH 20\n");
    writeLogs("POP bx\n");
    writeLogs("PUSH 0\n");
    writeLogs("PUSH 0\n");
    writeLogs("PUSH 0\n");
    writeLogs("POP ax\n");
    writeLogs("POP cx\n");
    writeLogs("POP dx\n");
    // bx contains the beginning of a free memory 

    writeLogs("JMP main\n");

    generateStmnt(tree->root, &global_name_space, nullptr); 

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


static int generateStmnt(Node *stmnt, Glob_Name_space *glob_name_space, Stack *name_space)
{
    assert(stmnt);
    assert(glob_name_space);

    if (stmnt->left_child)
    {   
        generateStmnt(stmnt->left_child, glob_name_space, name_space);
    }

    if (name_space)
    {
        //SHOW_NAMES(name_space);
    }

    Node *node = stmnt->right_child;
    auto node_type = node->node_type.bytes;

    if (node_type.is_operator && node->value.str[0] == '=')
    {
        PRINT_LINE;
        Node *left     = node->left_child;
        auto left_type = left->node_type.bytes;

        if (!left_type.is_variable)
        {
            SYNTAX_ERR;
        }

        //SHOW_NAMES(name_space);

        generateMath(node->right_child, glob_name_space, name_space);

        //SHOW_NAMES(name_space);

        // adding var in name_space
        // !!!!make function!!!!
        //use nameDtor
        Name *new_var = nameCtor(); 

        //SHOW_NAMES(name_space);


        strcpy(new_var->name, left->value.str);
        printf("newvar name = %s\n", new_var->name);
        //strcpy(new_var->func_name, "GLOBAL");
        new_var->Ntype.is_var = 1;
        //
        
        int shift = -1;
        if (name_space) //  try to find in locals
        {
            PRINT_LINE;
            printf("Name to find = %s\n", new_var->name);
            shift      = findElem(name_space, new_var->name);
            fprintf(stderr, "shift = %d\n", shift);
        }
        PRINT_LINE;
        int glob_shift = findElem(&(glob_name_space->global_vars), new_var->name);

        if (name_space || (shift == -1)) //for local vars
        {
            if (shift == -1) //if doesn't exist
            {
                stackPush(name_space, *new_var);

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
            stackPush(&(glob_name_space->global_vars), *new_var);
            
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
        PRINT_LINE;
        generateDefine(node, glob_name_space,name_space);
    }
    else if (node_type.is_keyword == IS_RETURN)
    {
        PRINT_LINE;
        generateMath(node->right_child, glob_name_space,name_space);
        
        writeLogs("POP cx\n");
        writeLogs(":RET\n");
    }
    else if (node_type.is_keyword == IS_CALL)
    {
        PRINT_LINE;
        generateCall(node, glob_name_space, name_space);
    }
    else if (node_type.is_keyword == IS_IF)
    {
        PRINT_LINE;
        generateIf(node, glob_name_space, name_space);
    }
    else if (node_type.is_keyword == IS_PRINTF)
    {
        PRINT_LINE;
        generatePrintf(node, glob_name_space, name_space);
    }
    else if (node_type.is_keyword == IS_SCANF)
    {
        PRINT_LINE;
        generateScanf(node, glob_name_space, name_space);
    }

    PRINT_LINE;
    return 0;
}


static int generateGlobal(Node *node, Glob_Name_space *glob_name_space, Stack *name_space, int *iter, int is_init)
{
    assert(node);
    assert(name_space);
    assert(iter);

    //Name *glob = findElem(name_space, node->value.str);
    //
    //if (glob)
    //{
    //
    //}

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


static int generateDefine(Node *node, Glob_Name_space *glob_name_space, Stack *name_space)
{    
    assert(node);
    assert(glob_name_space);
    if (KWORD_TYPE(node) != IS_DEFINE)
    {
        printf("!!! ERROR Node in generateDefine doesn't have define type !!!\n");
        return -1;
    }
    if (name_space)
    {
        printf("!!! ERROR Can't define a function inside another !!!\n");
        SYNTAX_ERR;
    }


    Stack Name_space = {};
    stackCtor(&Name_space, 0);

    Node *func_node = node->left_child;
    PRINT_LINE;
    printf("func name = %s\n", func_node->left_child->value.str);
    printf("is_args = %d\n", func_node->right_child != nullptr);
    // хорошая идея, но пока не надо
    addNewFunc(glob_name_space, func_node->left_child->value.str);
    PRINT_LINE;
    if (func_node->right_child)
    {
        PRINT_LINE;
        makeArgs(func_node->right_child, &Name_space);
    }
    PRINT_LINE;
    writeLogs(":%s\n", func_node->left_child->value.str);

    SHOW_NAMES(&Name_space);

    if (node->right_child)
    {
        PRINT_LINE;
        generateStmnt(node->right_child, glob_name_space, &Name_space);
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
    writeLogs("PUSH ax\n");
    writeLogs("POP [bx + ax]\n");
    INC_VAR_AMNT;

    //if (args)
    //{
    //    generateArgs(args, glob_name_space, name_space);
    //}

    writeLogs("PUSH bx\n");
    writeLogs("POP dx\n");

    // writeLogs("PUSH ax\n");
    // writeLogs("PUSH dx\n");
    // writeLogs("SUB\n");
    // writeLogs("POP ax\n");

    // изменим значение bx
    writeLogs("PUSH ax\n");
    writeLogs("PUSH bx\n");
    writeLogs("ADD\n");
    writeLogs("POP bx\n");

    // занулим ax
    writeLogs("PUSH 0\n");
    writeLogs("POP ax\n");

    if (args)
    {
       generateArgs(args, glob_name_space, name_space);
    }

    writeLogs(":CALL %s\n", func_node->left_child->value.str);
    writeLogs("PUSH cx\n");

    // выставляем нужное значение ax
    writeLogs("PUSH [bx - 1]\n");
    writeLogs("POP ax\n");

    // изменяем значение bx на предыдущее
    writeLogs("PUSH bx\n");
    writeLogs("PUSH ax\n");
    writeLogs("SUB\n");
    //bx на 1 больше, чем надо
    //т.к. однуячейку мы забирали для хранения ax
    writeLogs("PUSH 1\n");
    writeLogs("SUB\n");
    writeLogs("POP bx\n");

    return 0;
}


/*!
    \brief A function for generating arguments to a function
    \param [Node *]node Poiter to first param node
    \param [int *]iter Number of cmd
*/
static int generateArgs(Node *node, Glob_Name_space *glob_name_space, Stack *name_space)
{
    assert(node);
    assert(glob_name_space);
    assert(name_space);

    if (node->left_child)
    {
        generateArgs(node->left_child, glob_name_space, name_space);
    }

    Node *param      = node->right_child;

    auto  param_type = param->node_type.bytes;

    if (param_type.is_number)
    {
        writeLogs("PUSH %lg\n", param->value.number);
    }
    else if (param_type.is_variable)
    {
        //различать локальные и глобальные переменные т.к. локальные 
        //завязаны на bx
        char *name = param->value.str;

        int index = findElem(name_space, name);
        if (index >= 0)
        {
            writeLogs("PUSH [dx + %d]\n", index);
        }
        else
        {
            index = findElem(&(glob_name_space->global_vars), name);

            if (index >= 0)
            {
                PRINT_LINE;
                writeLogs("!!!!GLOBAL2\n");
                writeLogs("PUSH [%d]\n", index);
            }
            else
            {
                printf("!!! ERROR Can't find variable \"%s\" !!!\n", name);
                SYNTAX_ERR;
            }
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
    //INC_DX;
    node = node->left_child;

    return 0;
}


/*!
    \brief  Function for searching vars in any 
            name_space
    \return index if var exists, -1 if not
*/
static int getIndex(Glob_Name_space *global_name_space, Stack *name_space, char *name)
{
    assert(name_space);
    assert(name);

    int index = findElem(name_space, name);

    if (index == -1)
    {
        index = findElem(&(global_name_space->global_vars), name);
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
static int generateMath(Node *node, Glob_Name_space *glob_name_space, Stack *name_space)
{
    assert(node);
    assert(name_space);
    assert(glob_name_space);

    PRINT_LINE;

    bool is_call = (node->node_type.bytes.is_keyword == IS_CALL);
    
    if (node->left_child && !is_call)
    {
        generateMath(node->left_child, glob_name_space, name_space);
    }
    if (node->right_child && !is_call)
    {
        generateMath(node->right_child, glob_name_space, name_space);
    }

    auto node_type = node->node_type.bytes;

    int curr_if = glob_name_space->ifs.data[glob_name_space->ifs.data_size - 1];

    PRINT_LINE;

    //SHOW_NAMES(name_space);

    if (node_type.is_number)
    {
        PRINT_LINE;
        //printf("value = %lg ptr = %p\n", node->value.number, node);
        writeLogs("PUSH %lg\n", node->value.number);
    }
    else if (node_type.is_keyword == IS_CALL)
    {
        PRINT_LINE;
        generateCall(node, glob_name_space, name_space);
    }
    else if (node_type.is_operator)
    {
        //printf("node value = %s\n", node->value.str);
        PRINT_LINE;
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
            case '>':
                if (node->value.str[1] == '=')
                {
                    writeLogs("JAE TRUE%d\n", curr_if);
                    writeLogs("JMP FALSE%d\n", curr_if);
                }
                else
                {
                    writeLogs("JA TRUE%d\n", curr_if);
                    writeLogs("JMP FALSE%d\n", curr_if);
                }
                break;
            case '<':
                if (node->value.str[1] == '=')
                {
                    writeLogs("JBE TRUE%d\n", curr_if);
                    writeLogs("JMP FALSE%d\n", curr_if);
                }
                writeLogs("JB TRUE%d\n", curr_if);
                writeLogs("JMP FALSE%d\n", curr_if);
                break;
            case '=':
                if (node->value.str[1] == '=')
                {
                    writeLogs("JE TRUE%d\n", curr_if);
                    writeLogs("JMP FALSE%d\n", curr_if);
                }
                else
                {
                    SYNTAX_ERR;
                }
                break;
            case '!':
                if (node->value.str[1] == '=')
                {
                    writeLogs("JNE TRUE%d\n", curr_if);
                    writeLogs("JMP FALSE%d\n", curr_if);
                }
                else
                {
                    SYNTAX_ERR;
                }
                break;
        } 
    }
    else if (node_type.is_variable)
    {
        int shift = findElem(name_space, node->value.str);

        //for (int i = name_space->size - 1; i >= 0; i--)
        //{
        //    printf("var name = %s, index = %d\n", name_space->data[i].name, i);
        //}
        
        if (shift == -1)
        {
            shift = findElem(&(glob_name_space->global_vars), node->value.str);

            if (shift == -1)
            {
                printf("!!! ERROR Undefined var %s !!!\n", node->value.str);
                SYNTAX_ERR;
            }
            
            //if exist glob var
            printf("!!! GLOBAL\n");
            writeLogs("!!!!GLOBAL1\n");
            writeLogs("PUSH [%d]\n", shift);
            
            return 0;
        }
        //if exist var
        writeLogs("PUSH [bx + %d]\n", shift);
    }
    else if (node->node_type.number == IS_SQRT)
    {
        writeLogs("SQRT\n");
    }

    //SHOW_NAMES(name_space);

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

    PRINT_LINE;
    printf("stck sz = %d\n", name_space->size);
    printf("stack data = %p\n", name_space->data);

    //SHOW_NAMES(name_space);

    for (int i = name_space->size - 1 ; i >= 0; i--)
    {
        Name name_member = name_space->data[i];
        printf("i = %d\n", i);
        printf("name_member name = %s\n", name_space->data[i].name);
        printf("elem = %s\n", elem);
        PRINT_LINE;
        if (!strcmp(name_member.name, elem))
        {
            PRINT_LINE;
            return (name_member.Ntype.is_func) * (-2) + name_member.Ntype.is_var * i;
        }
        PRINT_LINE;
    }
    PRINT_LINE;
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

    PRINT_LINE;

    while (node)
    {   
        Node *right = node->right_child;

        Name *new_arg = nameCtor();
        strcpy(new_arg->name, right->value.str);
        new_arg->Ntype.is_var = 1;

        stackPush(name_space, *new_arg);
        
        nameDtor(new_arg);
        node = node->left_child;
    }

    //SHOW_NAMES(name_space);

    return 0;
}


static int addNewFunc(Glob_Name_space *global_name_space, char *name)
{
    assert(global_name_space);
    assert(name);

    CHECK_NAME(global_name_space, name, 1);

    Name *new_func = nameCtor();

    strcpy(new_func->name, name);
    new_func->Ntype.is_func = 1;

    stackPush(&(global_name_space->func), *new_func);

    return 0;
}


static int nameDtor(Name *name)
{
    assert(name);

    free(name->name);
    free(name);

    return 0;
}


static int showNames(Stack *name_space)
{
    assert(name_space);

    printf("Stack = %p, stk.data = %p ", name_space, name_space->data);
    printf("size = %d, capacity = %d\n", name_space->size, name_space->capacity);
    for (int i = name_space->size - 1; i >= 0; i--)
    {
        printf("name%d = \n", i);
        printf("%s\n", name_space->data[i].name);
    }

    return 0;
}


static int generateIf(Node *node, Glob_Name_space *glob_name_space, Stack *name_space)
{
    assert(node);
    assert(glob_name_space);
    assert(name_space);

    //IF_AMNT++;
    IfStack* if_stack = &(glob_name_space->ifs);
    if_stack->if_amnt++;
    if_stack->data[if_stack->data_size] = if_stack->if_amnt;
    if_stack->data_size++;

    int if_amnt = if_stack->if_amnt;

    Node *decision = node->right_child;

    generateMath(node->left_child, glob_name_space, name_space);

    writeLogs(":TRUE%d\n", if_amnt);
    generateStmnt(decision->left_child, glob_name_space, name_space);
    writeLogs("JMP END%d\n", if_amnt);

    writeLogs(":FALSE%d\n", if_amnt);
    if (decision->right_child)
    {
        generateStmnt(decision->right_child, glob_name_space, name_space);
    }
    writeLogs("JMP END%d\n", if_amnt);

    writeLogs(":END%d\n", if_amnt);

    if_stack->data_size--;
    if_stack->data[if_stack->data_size] = 0;

    return 0;
}


static int generatePrintf(Node *node,  Glob_Name_space *glob_name_space, Stack *name_space)
{
    assert(node);
    assert(glob_name_space);
    assert(name_space);
    assert(node->right_child);

    generateMath(node->right_child, glob_name_space, name_space);

    writeLogs("OUT\n");
    writeLogs("POP\n");

    return 0;
}


static int generateScanf(Node *node, Glob_Name_space *glob_name_space, Stack *name_space)
{
    assert(node);
    assert(glob_name_space);
    assert(name_space);
    assert(node->right_child);

    Node *rigth = node->right_child;

    if (!rigth->node_type.bytes.is_variable)
    {
        fprintf(stderr, "Can't make scanf for number of math expression!!!\n");
        SYNTAX_ERR;
    }

    writeLogs("IN\n");

    char *name = rigth->value.str;

    int index = findElem(name_space, name);
    if (index >= 0)
    {
        writeLogs("POP [bx + %d]\n", index);
    }
    else
    {
        index = findElem(&(glob_name_space->global_vars), name);
        if (index >= 0)
        {
            PRINT_LINE;
            writeLogs("!!!!GLOBAL2\n");
            writeLogs("POP [%d]\n", index);
        }
        else
        {
            printf("!!! ERROR Can't find variable \"%s\" !!!\n", name);
            SYNTAX_ERR;
        }
    }
}