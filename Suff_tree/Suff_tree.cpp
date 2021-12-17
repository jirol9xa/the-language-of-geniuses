#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <cmath>
#include "LogsLib.h"
#include "Suff_tree.h"
#include "Syntax.h"


extern const char *RESERVED_WORDS;
extern const int   LETTERS_AMOUNT;

const int IS_IF     = 1;
const int IS_WHILE  = 1 << 2;
const int IS_RETURN = 1 << 2 + 1;
const int IS_CONST  = 1 << 3;
const int IS_MAIN   = 1 << 3 + 1;
const int IS_DEFINE = 1 << 4;



static void printSuffNode(Suff_node *node);
static void printEdge(Edge *edge);
static int *addWord(Edge *edge, int iter, int wrd_size, int status);
static int suffSearch(char *cmd, Suff_node *node, int iter);
static int myStrcmp(const char *string, int end1, const char *standart, int end2);


Suff_node *suffNodeCtor()
{
    return (Suff_node *) calloc(1, sizeof(Suff_node));
}


Edge *edgeCtor()
{
    return (Edge *) calloc(1, sizeof(Edge));
}


//int suffDump(Suff_Tree *tree)
//{
//    assert(tree);
//    
//    openLogs("LOGS/suff_logs.dot");  
//    writeLogs("digraph G{\n");
//
//    printSuffNode(tree->root);
//
//    writeLogs("}\n");
//
//    closeLogs();
//
//    system("dot -T png LOGS/logs.dot -o pic.png");
//    system("eog pic.png");
//
//    return 0;
//}
//
//
//static void printSuffNode(Suff_node *node)
//{
//    assert(node);
//
//    writeLogs("    elem_%x[style = \"filled\", shape = record,fillcolor = \"#48D1CC\", label = \"", node);
//    for (int i = 0; i < LETTERS_AMOUNT; i++)
//    {
//        if (node->edges[i])
//        {
//            PRINT_LINE;
//            printSuffNode(node->edges[i]->next);
//
//            writeLogs("    elem_%x -> elem_%x [label = \"", node, node->edges[i]->next);
//            printEdge(node->edges[i]);
//            writeLogs("\"color = \"blue\"];\n");
//        }
//    }
//}


static void printEdge(Edge *edge)
{
    assert(edge);

    for (int i = 0; i < edge->length; i++)
    {
        writeLogs("%c", RESERVED_WORDS[edge->start + i]);
    }
}


int suffNodeDtor(Suff_node *node)
{
    assert(node);

    for (int i = 0; i < LETTERS_AMOUNT; i++)
    {
        if (node->edges[i])
        {
            if (node->edges[i]->next)
            {
                suffNodeDtor(node->edges[i]->next);
            }
            free(node->edges[i]);
        }
    }

    free(node);
    return 0;
}


Suff_Tree *suffTreeCtor()
{
    Suff_Tree *tree = (Suff_Tree *) calloc(1, sizeof(Suff_Tree));
    tree->root = suffNodeCtor();

    int size = strlen(RESERVED_WORDS);
    int i = 0;

    while (RESERVED_WORDS[i] != '\0')
    {
        int wrd_size = 0;

        while (RESERVED_WORDS[wrd_size + i] >= 'a' && RESERVED_WORDS[wrd_size + i] <= 'z')
        {
            wrd_size++;
        }

        int status = 0;

        sscanf(RESERVED_WORDS + wrd_size + i, "%d", &status);

        Edge **edge = &(tree->root->edges[RESERVED_WORDS[i] - 'a']);

        if (*edge == nullptr)
        {
            *edge           = edgeCtor();
            (*edge)->start  = i;
            (*edge)->length = wrd_size;
            (*edge)->status = status;
        
            i += wrd_size + 2 + (int) log10(status);

            continue;
        }

        addWord(*edge, i, wrd_size, status);

        i += wrd_size + 2 + (int) log10(status);
    }

    return tree;
}


static int *addWord(Edge *edge, int iter, int wrd_size, int status)
{
    assert(edge);

    if (edge->next)
    {
        addWord(edge->next->edges[RESERVED_WORDS[iter + 1] - 'a'], iter + 1, wrd_size - 1, status);

        return 0;
    }

    edge->next = suffNodeCtor();

    Edge **new_edge = &(edge->next->edges[RESERVED_WORDS[iter + 1] - 'a']);
    Edge **old_edge = &(edge->next->edges[RESERVED_WORDS[edge->start + 1] - 'a']);

    *new_edge = edgeCtor();
    *old_edge = edgeCtor();

    (*old_edge)->start  = edge->start + 1;
    (*old_edge)->length = edge->length - 1;
    (*old_edge)->status = edge->status;

    edge->length = 1;
    edge->status = 0;

    (*new_edge)->start  = iter + 1;
    (*new_edge)->length = wrd_size - 1;
    (*new_edge)->status = status;

    return 0;
}


void printSuffTree(Suff_node *node)
{
    assert(node);

    for (int i = 0; i < LETTERS_AMOUNT; i++)
    {
        if (node->edges[i])
        {
            Edge *edge = node->edges[i];
            printf("start = %d, len = %d, next = %p, begin = %c, end = %c, status = %d\n", edge->start, edge->length, edge->next, 
                    RESERVED_WORDS[edge->start], RESERVED_WORDS[edge->start + edge->length - 1], edge->status);

            if (edge->next)
            {
                printf("son\n");
                
                printSuffTree(edge->next);
            }
        }
    }
}


int isKeyword(char *cmd, Suff_Tree *tree)
{
    assert(cmd);
    assert(tree);

    return !suffSearch(cmd, tree->root, 0);
}


static int suffSearch(char *cmd, Suff_node *node, int iter)
{
    assert(cmd);
    assert(node);
    if (abs(cmd[iter] - 'a') > 26)
    {
        return -1;
    }

    Edge *edge = node->edges[cmd[iter] - 'a'];

    if (edge && (iter + edge->length) <= strlen(cmd))
    {
        if (myStrcmp(cmd + iter, strlen(cmd), RESERVED_WORDS + edge->start, edge->length))
        {
            return -1;
        }

        iter += edge->length;

        if (edge->next)
        {
            return suffSearch(cmd, edge->next, iter);
        }
    }

    if (!edge || edge->next || iter != strlen(cmd))
    {
        return -1;
    }
    
    return 0;
}


static int myStrcmp(const char *string, int end1, const char *standart, int end2)
{
    assert(string);
    
    int i = 0;
    for (; i < end2; i++)
    {
        if (i > end1 || string[i] != standart[i])
        {
            PRINT_LINE;
            return -1;
        }
    }

    return 0;
}