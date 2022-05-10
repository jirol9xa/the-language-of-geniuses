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
    //if (argc < 2)
    //{
    //    printf("!!! ERROR Can't run without sourse file !!!\n");
    //    return -1;
    //}
    
    FILE *sourse = fopen("src"/*argv[1]*/, "r");
    if (!sourse)
    {
        fprintf(stderr, "Can't open src file, sourse = %p!!!\n", sourse);
        return 0;
    }
    fprintf(stderr, "src = %p\n", sourse);

    Tree tree = {};

    readCode(sourse, &tree);
    
    fclose(sourse);
    
    treeDtor(&tree);

    //system("cd .. && cd .. && cd cpu && cd ASM && ./asm ../../language/run/ASM.txt && cd .. && cd CPU && ./cpu");
    return 0;
}