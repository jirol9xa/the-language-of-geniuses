#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "Tree.h"
#include "Logger.h"
#include "Syntax.h"
#include "Suff_tree.h"
#include "TreeFile.h"
#include "TextLib.h"


static int writeNode     (Node *node);
static int writeNodeValue(Node *node);
static int readArg(Node *node, char *text, Suff_Tree *suff_tree);


int writeTree(Tree *tree, const char *file_name)
{
    assert(tree);
    IS_DTRCT(tree);

    openLogs(file_name);

    writeNode(tree->root);

    closeLogs();

    return 0;
}


static int writeNode(Node *node)
{
    assert(node);

    writeLogs("(");

    if (node->left_child)
    {
        writeNode(node->left_child);
    }

    writeNodeValue(node);

    if (node->right_child)
    {
        writeNode(node->right_child);
    }

    writeLogs(")");

    return 0;
}


static int writeNodeValue(Node *node)
{
    assert(node);

    if (node->node_type.bytes.is_number)
    {
        writeLogs("%lg", node->value.number);
    }
    else if (node->node_type.bytes.is_variable || node->node_type.bytes.is_func)
    {
        writeLogs("'%s'", node->value.str);
    }
    else
    {
        writeLogs("%s", node->value.str);
    }

    return 0;
}


int readTree(Tree *tree, const char *file_name, Suff_Tree *suff_tree)
{
    assert(tree);
    assert(file_name);
    IS_DTRCT(tree);

    FILE *sourse = fopen(file_name, "r");

    char *text = readBuff(sourse);

    Node *cur_node = tree->root;
    long  num_symb = strlen(text);

    for (int i = 0; i < num_symb && text[i] != '\0'; i++)
    {
        i += skipSpace(text + i);

        if (text[i] == '(')
        {
            if (tree->status.empty_tree)
            {
                tree->status.empty_tree = 0;
                continue;
            }

            tree->size ++;

            if (!cur_node->node_type.number)
            {
                cur_node = nodeCtor(cur_node, cur_node->left_child, 1);
            }
            else
            {
                cur_node = nodeCtor(cur_node, cur_node->right_child, 0);
            }
            //else
            //{
            //    printf("!!! ERROR Node can't has more than two children !!!\n");
            //    tree->status.destructed_tree = 1;
            //    return -1;
            //}
        }
        else if (text[i] == ')')
        {   
            if (cur_node->parent)
            {
                cur_node = cur_node->parent;
            }
        }
        else
        {
            i += readArg(cur_node, text + i, suff_tree) - 1;
        }
    }

    free(text);
    fclose(sourse);
    return 0;
}


static int readArg(Node *node, char *text, Suff_Tree *suff_tree)
{
    assert(node);
    assert(text);

    double number = 0;
    int i = 0;

    i += skipSpace(text);

    int size = 0; 

    if (sscanf(text + i, "%lg%n", &number, &size))
    {
        node->node_type.bytes.is_number = 1;
        node->value.number = number;

        return i + size;
    }
    if (text[i] == '\'')
    {
        i++;

        sscanf(text + i, "%m[A-Za-z-_0-9]%n", &(node->value.str), &size);
 
        int prnt_stat = 0;

        if (node->parent)
        {
            prnt_stat = (node->parent->node_type.bytes.is_keyword == IS_FUNCTION);
        }

        // filling status for every type of node 
        node->node_type.number = 1 << 9 + prnt_stat;

        return i + size + 1;
    }

    sscanf(text + i, "%m[a-zA-Z_*=<>&|+-0-9]%n", &(node->value.str), &size);

    int status = isKeyword(node->value.str, suff_tree);
    if (!strcmp(node->value.str, "call"))
    {
        PRINT_RESHETKA;
        PRINT_LINE;
        printf("status = %d\n", status);
    }

    node->node_type.number = status + !status * (1 << 7);

    return i + size;
}