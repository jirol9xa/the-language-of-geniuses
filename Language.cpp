#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tree.h"
#include "Syntax.h"
#include "LogsLib.h"
#include "TextLib.h"
#include "Recursive_descent.h"
#include "Language.h"
#include "TreeFile.h"

char *RESERVED_WORDS = "if1$while2$return3$sin9$cos10$ln11$main5$define6$const4$"; //fix codes
int LETTERS_AMOUNT = 26;
// hello vovqa, i love you so fuckin much

int readCode(FILE *sourse, Tree *tree)
{
    assert(sourse);
    assert(tree);
    if (tree->status.destructed_tree)
    {
        printf("!!! ERROR Can't work with destructed tree !!!\n");
        return -1;
    }

    // мб юзать библиотеку онегина и перегон в построчный текст

    long int file_length = 0;
    fileLength(&file_length, sourse);

    char *text = (char *)calloc(file_length + 1, sizeof(char));
    long int num_symb = fread(text, sizeof(char), file_length, sourse);

    text[num_symb] = '\0';

    constructTree(tree, text);

    free(text);

    writeTree(tree, "My_tree");

    return 0;
}
