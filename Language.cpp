#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
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


char *RESERVED_WORDS = nullptr;//"if8193$else8204$while2$return8195$sin10$cos11$ln12$sqrt13$main5$const4$statement8199$define8198$func8200$decision8202$param8203$call8201$printf8205$scanf8206$"; //fix codes
int LETTERS_AMOUNT = 26;


static int fillResWords(FILE *cmd_file);


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

    FILE *cmd_file = fopen("../cmd_file.txt", "r");
    assert(cmd_file); 

    fillResWords(cmd_file);

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


static int skipSymbs(char *buff)
{
    assert(buff);

    int i = 0;

    while (buff[i] != '$')  i++;
    
    return i + 1;    
}


static char *itoa10(int arg, char *buff)
{
    fprintf(stderr, "arg = %d, ", arg);

    buff[0] = arg / 1000 + '0';
    arg %= 1000;
    buff[1] = arg / 100 + '0';
    arg %= 100;
    buff[2] = arg / 10 + '0';
    arg %= 10;
    buff[3] = arg + '0';

    int i = 0;

    buff[4] = '\0';
    fprintf(stderr, "buff = %s\n", buff);

    return buff;
}


static int fillResWords(FILE *cmd_file)
{
    assert(cmd_file);

    int  res_cap = 100;
    int  res_len = 0;
    RESERVED_WORDS = (char *) calloc(res_cap + 1, sizeof(char));
    assert(RESERVED_WORDS);

    long file_length = 0;
    fileLength(&file_length, cmd_file);

    char *buff = (char *) calloc(file_length + 1, sizeof(char));
    fread(buff, sizeof(char), file_length, cmd_file);
    assert(buff);
    buff[file_length] = '\0';

    char cmd[32]     = {};
    int  code        = 0;
    char str_code[5] = {}; 

    while (*buff != '\0')
    {
        buff += skipSymbs(buff);
        
        int cmd_length = strlen(cmd);
        res_len += cmd_length;
        buff    += cmd_length;
        
        buff += skipSymbs(buff);
        buff += skipSymbs(buff);
        
        sscanf(buff, " %d ", &code);  // scanning cmd code
        res_len += 4;
        buff    += 4;

        buff += skipSymbs(buff);
        buff++;

        if (res_len >= res_cap - 1)
        {
            res_cap *= 2;

            void *temp_ptr = realloc(RESERVED_WORDS, res_cap * sizeof(char));
            assert(temp_ptr);
            RESERVED_WORDS = (char *) temp_ptr;
        }
        
        strcat(RESERVED_WORDS, cmd);
        strcat(RESERVED_WORDS, itoa10(code, str_code));
        RESERVED_WORDS[res_len++] = '$';
    }

    RESERVED_WORDS[res_len] = '\0';
    
    return 0;
}  

