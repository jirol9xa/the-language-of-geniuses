#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Syntax.h"
#include "Tree.h"
#include "Logger.h"
#include "Language.h"
#include "Suff_tree.h"


int main(const int argc, const char **argv)
{
    if (argc < 2)
    {
        printf("!!! ERROR Can't run without sourse file !!!\n");
        return -1;
    }
    
    FILE *sourse = fopen(argv[1], "r");

    Tree tree = {};

    readCode(sourse, &tree);
    
    fclose(sourse);
    
    treeDtor(&tree);
    //free(tree.root);
    return 0;
}