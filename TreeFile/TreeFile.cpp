#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tree.h"
#include "LogsLib.h"
#include "TreeFile.h"


static int writeNode     (Node *node);
static int writeNodeValue(Node *node);


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

    if (node->left_child)
    {
        writeLogs("(");
        writeNode(node->left_child);
        writeLogs(")");
    }

    writeNodeValue(node);

    if (node->right_child)
    {
        writeLogs("(");
        writeNode(node->right_child);
        writeLogs(")");
    }

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
