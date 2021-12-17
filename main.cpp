#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Syntax.h"
#include "Tree.h"
#include "LogsLib.h"
#include "Language.h"
#include "Suff_tree.h"

extern int   LETTERS_AMOUNT;
extern char *RESERVED_WORDS;

int main(const int argc, const char **argv)
{
    if (argc < 2)
    {
        printf("!!! ERROR Can't run without sourse file !!!\n");
        return -1;
    }
    
    FILE *sourse = fopen(argv[1], "r");

    Tree tree = {};
    treeCtor(&tree);

    readCode(sourse, &tree);
    
    treeDtor(&tree);
    return 0;
}