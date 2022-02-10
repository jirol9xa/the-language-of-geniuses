#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <cmath>
#include "Logger.h"
#include "Suff_tree.h"
#include "Syntax.h"


extern const char *RESERVED_WORDS;
extern int         LETTERS_AMOUNT;


static void printSuffNode(Suff_node *node);
static void printEdge    (Edge *edge);
static int *addWord      (Edge *edge, int iter, int wrd_size, int status);
static int  suffSearch   (char *cmd, Suff_node *node, int iter);
static int  myStrcmp     (const char *string, int end1, const char *standart, int end2);
static int  suffNodeDtor (Suff_node *node);
static int  min          (int a, int b);


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
//    openLogs("../LOGS/suff_logs.dot");  
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
//            
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


int suffTreeDtor(Suff_Tree *tree)
{
    assert(tree);

    suffNodeDtor(tree->root);
    free(tree);

    return 0;
}


static int suffNodeDtor(Suff_node *node)
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

    int i = 0;

    while (RESERVED_WORDS[i] != '\0')
    {
        int wrd_size = 0;

        while (RESERVED_WORDS[wrd_size + i] >= 'a' && RESERVED_WORDS[wrd_size + i] <= 'z')
        {
            wrd_size++;
        }

        int status   = 0;
        int num_size = 0;

        sscanf(RESERVED_WORDS + wrd_size + i, "%d%n", &status, &num_size);

        Edge **edge = &(tree->root->edges[RESERVED_WORDS[i] - 'a']);

        if (*edge == nullptr)
        {
             *edge          = edgeCtor();
            (*edge)->start  = i;
            (*edge)->length = wrd_size;
            (*edge)->status = status;

            i += wrd_size + 1 + num_size;

            continue;
        }

        addWord(*edge, i, wrd_size, status);

        i += wrd_size + 1 + num_size;
    }

    //printSuffTree(tree->root);

    return tree;
}


static int *addWord(Edge *edge, int iter, int wrd_size, int status)
{
    assert(edge);

    if (edge->next && edge->next->edges[RESERVED_WORDS[iter + 1] - 'a'])
    {
        addWord(edge->next->edges[RESERVED_WORDS[iter + 1] - 'a'], iter + 1, wrd_size - 1, status);

        return 0;
    }

    int        shift    = 0;
    int        length   = edge->length;
    Edge      *cur_edge = edge;
    Suff_node *cur_node = nullptr;

    if (!edge->next)
    {
        #define NEXT_EDGE(arg) cur_edge->next->edges[RESERVED_WORDS[arg] - 'a']

        while (RESERVED_WORDS[edge->start + shift] == RESERVED_WORDS[iter + shift] && shift < min(length, wrd_size))
        {
            shift++;

            cur_edge->next = suffNodeCtor();
            cur_node       = cur_edge->next;

            NEXT_EDGE(cur_edge->start + 1) = edgeCtor();

            Edge *old_edge = NEXT_EDGE(cur_edge->start + 1);

            old_edge->start  = cur_edge->start  + 1;
            old_edge->length = cur_edge->length - 1;    
            old_edge->status = cur_edge->status;

            cur_edge->length = 1;
            cur_edge->status = 0;

            cur_edge = old_edge;
        }

        #undef NEXT_EDGE
    }
    else
    {
        cur_node = edge->next;
        shift++;
    }

    Edge **new_edge = &(cur_node->edges[RESERVED_WORDS[iter + shift] - 'a']);

     *new_edge          = edgeCtor();
    (*new_edge)->length = wrd_size - shift;
    (*new_edge)->start  = iter + shift;
    (*new_edge)->status = status;

    //edge->next = suffNodeCtor();
    //
    //Edge **new_edge = &(edge->next->edges[RESERVED_WORDS[iter + 1] - 'a']);
    //Edge **old_edge = &(edge->next->edges[RESERVED_WORDS[edge->start + 1] - 'a']);
    //
    //*new_edge = edgeCtor();
    //*old_edge = edgeCtor();
    //
    //(*old_edge)->start  = edge->start + 1;
    //(*old_edge)->length = edge->length - 1;
    //(*old_edge)->status = edge->status;
    //
    //edge->length = 1;
    //edge->status = 0;
    //
    //(*new_edge)->start  = iter + 1;
    //(*new_edge)->length = wrd_size - 1;
    //(*new_edge)->status = status;

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
            printf("start = %d, len = %d, next = %p, status = %d\n", edge->start, edge->length, edge->next, edge->status);

            for (int i = 0; i < edge->length; i++)
            {
                printf("%c", RESERVED_WORDS[edge->start + i]);
            }

            printf("\n");

            if (edge->next)
            {
                printf("son\n");
                
                printSuffTree(edge->next);

                printf("children finished\n");
            }
        }
    }
}


int isKeyword(char *cmd, Suff_Tree *tree)
{
    assert(cmd);
    assert(tree);

    return suffSearch(cmd, tree->root, 0);
}


static int suffSearch(char *cmd, Suff_node *node, int iter)
{
    assert(cmd);
    assert(node);
    if (abs(cmd[iter] - 'a') > 26)
    {
        return 0;
    }

    Edge *edge = node->edges[cmd[iter] - 'a'];

    if (edge && (iter + edge->length) <= strlen(cmd))
    {
        if (myStrcmp(cmd + iter, strlen(cmd), RESERVED_WORDS + edge->start, edge->length))
        {
            return 0;
        }

        iter += edge->length;

        if (edge->next)
        {
            
            return suffSearch(cmd, edge->next, iter);
        }
    }

    if (!edge || edge->next || iter != strlen(cmd))
    {
        return 0;
    }
          
    return edge->status;
}


static int myStrcmp(const char *string, int end1, const char *standart, int end2)
{
    assert(string);
    
    int i = 0;
    for (; i < end2; i++)
    {
        if (i > end1 || string[i] != standart[i])
        {
            
            return -1;
        }
    }

    return 0;
}


static int min(int a, int b)
{
    return (a < b) ? a : b;
}