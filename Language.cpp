#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tree.h"
#include "Suff_tree.h"
#include "Syntax.h"
#include "Logger.h"
#include "TextLib.h"
#include "Recursive_descent.h"
#include "Language.h"
#include "TreeFile.h"
#include "Stack.h"
#include "Back_end.h"


const char *RESERVED_WORDS = "if8193$else8204$while2$return8195$sin10$cos11$ln12$sqrt13$main5$const4$statement8199$define8198$func8200$decision8202$param8203$call8201$printf8205$scanf8206$"; //fix codes
int LETTERS_AMOUNT = 26;


int readCode(FILE *sourse, Tree *tree)
{
    assert(sourse);
    assert(tree);
    IS_DTRCT(tree);

    long int file_length = 0;
    fileLength(&file_length, sourse);

    char *text = (char *) calloc(file_length + 1, sizeof(char));
    long int num_symb = fread(text, sizeof(char), file_length, sourse);

    text[num_symb] = '\0';

    Suff_Tree *suff_tree = suffTreeCtor();
        
    constructTree(tree, text, suff_tree);

    writeTree(tree, "My_tree");

    free(text);

    Tree tree2 = {};
    treeCtor(&tree2);

    readTree(&tree2, "My_tree", suff_tree);

    treeDump(&tree2);

    generateAsm(&tree2, "ASM.txt");

    suffTreeDtor(suff_tree);
    treeDtor(&tree2);
    return 0;
}
