//A.p Проверяй на C++
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tree.h"
#include "Back_end.h"
#include "List.h"
#include "Logger.h"
#include "Syntax.h"


static Name *nameCtor();
static int   generateGeneral (Tree *tree,  int *iter);
static int   generateStmnt   (Node *stmnt, List *name_space, int *iter);
static int   generateDefine  (Node *node,  List *name_space, int *iter);
static int   generateGlobal  (Node *node,  List *name_space, int *iter,   int is_init);
static int   generateVar     (Node *node,  List *name_space, int *iter,   int is_init);
static int   addNewName      (Node *node,  List *name_space, int  is_var, int *shift);
static int   generateCall    (Node *node,  List *name_space, int *iter);
static int   generateArgs    (Node *node,  List *name_space, int *iter);
static int   generateFuncCode(Node *node,  List *name_space);
static int   generateMath    (Node *node,  List *name_space, int *iter);
static int   writeNode       (Node *node,  int  *iter);
static int   getIndex        (List *name_space, char *name, int is_func);


#define KWORD_TYPE(arg) arg->node_type.bytes.is_keyword

#define SYNTAX_ERR             \
{                              \
    PRINT_RESHETKA;            \
    printf("Syntax err in\n"); \
    PRINT_RESHETKA;            \
                    \
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

    List name_space = {};
    listCtor(&name_space, 0);

    // creating const memory
    writeLogs("PUSH 20\n");
    writeLogs("POP bx\n");
    // bx contains the beginning of a free memory 

    writeLogs("JMP :main\n");
    generateStmnt(tree->root, &name_space, &iter);
    
    

    listDtor(&name_space);
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


static int generateStmnt(Node *stmnt, List *name_space, int *iter)
{
    assert(stmnt);
    assert(name_space);
    assert(iter);

    if (stmnt->left_child)
    {
        
        generateStmnt(stmnt->left_child, name_space, iter);
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

        generateMath(node->right_child, name_space, iter);
    }
    else if (node_type.is_keyword == IS_DEFINE)
    {
        
        generateDefine(node, name_space, iter);
    }
    else if (node_type.is_keyword == IS_RETURN)
    {
        generateMath(node->right_child, name_space, iter);
        
        writeLogs("POP cx\n");
        writeLogs(":RET\n");
    }

    return 0;
}


static int generateGlobal(Node *node, List *name_space, int *iter, int is_init)
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


static int generateVar(Node *node, List *name_space, int *iter, int is_init)
{
    assert(node);
    assert(name_space);
    assert(iter);

    Name *name = findElem(name_space, node->value.str);

    if (!is_init)
    {
        if (!name->type.is_var)
        {
            printf("!!! ERROR Can't perform assignment to a function !!!\n");
            SYNTAX_ERR;
        }

        writeLogs("PUSH [%d]", name->index.ram_indx);
        
        return 0;
    }

    writeLogs("POP [%d]\n", name->index.ram_indx);
    
    return 0;
}


static int writeNode(Node *node, int *iter)
{
    assert(node);
    assert(iter);

    return 0;
}


static int addNewName(Node *node, List *name_space, int is_var, int *shift)
{
    assert(node);
    assert(name_space);

    Name *var           =  nameCtor();
    var->type.is_var    =  is_var;
    var->type.is_func   =  !is_var;
    var->name           =  node->value.str;
    var->index.ram_indx = *shift;

    listPushBack(name_space, var);

    return 0;
}


static int generateDefine(Node *node, List *name_space, int *shift)
{    
    assert(node);
    if (KWORD_TYPE(node) != IS_DEFINE)
    {
        printf("!!! ERROR Node if generateDefine doesn't have define type !!!\n");
        return -1;
    }

    INC(shift, 1);

    //int start_size  = name_space->size;

    Node *func_node = node->left_child;

    addNewName(func_node->left_child, name_space, 0, shift);
    
    writeLogs(":%s\n", func_node->left_child->value.str);

    generateStmnt(node->right_child, name_space, shift);

    auto func_type = func_node->left_child->node_type.bytes;

    //writeLogs("POP [bx + ]")
    
    if (func_type.is_keyword == IS_MAIN)
    {
        writeLogs("HLT\n");
    } 

    return 0;  
}


static int generateCall(Node *node, List *name_space, int *iter)
{
    assert(node);
    assert(name_space);
    assert(iter);

    Node *func_node = node->right_child;

    if (!findElem(name_space, func_node->left_child->value.str))
    {
        SYNTAX_ERR;
    }

    Node *args = func_node->right_child;

    

    if (args)
    {
        generateArgs(args, name_space, iter);
    }

    writeLogs(":CALL %s\n", func_node->left_child->value.str);
    
    return 0;
}


/*!
    \brief A function for generating arguments to a function
    \param [Node *]node Poiter to first param node
    \param [int *]iter Number of cmd
*/
static int generateArgs(Node *node, List *name_space, int *iter)
{
    assert(node);
    assert(name_space);

    do
    {
        Node *param      = node->right_child;
        auto  param_type = param->node_type.bytes;

        if (param_type.is_number)
        {
            writeLogs("PUSH %d\n", param->value.number);
            writeLogs("POP [bx + %d]\n"); // вычислять смещение

            INC(iter, 4);

        }
        else if (param_type.is_variable)
        {
            int index = getIndex(name_space, param->value.str, 0);

            writeLogs("PUSH [%d]\n", index);
            //writeLogs("POP")
        }
        else if (param_type.is_keyword == IS_CALL)
        {
            
            generateCall(param, name_space, iter);
            //int index = getIndex(name_space, param->value.str, 1);
        }
    }
    while (node->left_child);
    
    return 0;
}


static int getIndex(List *name_space, char *name, int is_func)
{
    assert(name_space);
    assert(name);

    Name *name_info = findElem(name_space, name);

    if (is_func)
    {
        return name_info->index.asm_indx;
    }

    return name_info->index.ram_indx;
}


static int generateFuncCode(Node *node, List *name_space)
{
    assert(node);
    assert(name_space);

    int start_size = name_space->size;

    return 0;
}

static int generateMath(Node *node, List *name_space, int *iter)
{
    assert(node);
    assert(name_space);
    assert(iter);

    bool is_call = (node->node_type.bytes.is_keyword == IS_CALL);

    
    if (node->left_child && !is_call)
    {
        generateMath(node->left_child, name_space, iter);
    }
    if (node->right_child && !is_call)
    {
        generateMath(node->right_child, name_space, iter);
    }

    auto node_type = node->node_type.bytes;

    if (node_type.is_number)
    {
        printf("value = %lg ptr = %p\n", node->value.number, node);
        writeLogs("PUSH %lg\n", node->value.number);
    }
    else if (node_type.is_keyword == IS_CALL)
    {
        

        generateCall(node, name_space, iter);
    }
    else if (node_type.is_operator)
    {
        printf("node value = %s\n", node->value.str);

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

    return 0;
}