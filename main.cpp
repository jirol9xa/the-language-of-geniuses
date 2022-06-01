#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Syntax.h"
#include "Tree.h"
#include "Logger.h"
#include "Language.h"
#include "Suff_tree.h"
#include "Trans.h"

int main(const int argc, const char **argv)
{
    if (argc < 2)
    {
        printf("!!! ERROR Can't run without sourse file !!!\n");
        return -1;
    }
    
    //FILE *rus_src = fopen("rus_src", "r");
    //FILE *rus_dst = fopen("src", "w");
    //
    //makeTrans(rus_src, rus_dst);
    //
    //fclose(rus_src);
    //fclose(rus_dst);
    
    FILE *sourse = fopen(argv[1], "r");
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

    system("cd .. && cd .. && cd cpu && cd ASM && ./asm ../../language/run/ASM.txt");
    PRINT_LINE;
    system("cd .. && cd .. && cd cpu && cd CPU && ./cpu");
    return 0;
}