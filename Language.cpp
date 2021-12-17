#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tree.h"
#include "LogsLib.h"
#include "TextLib.h"
#include "Recursive_descent.h"
#include "Language.h"

    
char *RESERVED_WORDS = "if1$while2$return3$";
int   LETTERS_AMOUNT = 26;
    // hello vovqa, i love you so fuckin much


//int readCode(FILE *sourse, Tree *tree)
//{
//    assert(sourse);
//    assert(tree);
//    if (tree->status.destructed_tree)
//    {
//        printf("!!! ERROR Can't work with destructed tree !!!\n");
//        return -1;
//    }
//
//    // мб юзать библиотеку онегина и перегон в построчный текст
//
//    long int file_length = 0;
//    fileLength(&file_length, sourse);
//
//    char *text = (char *) calloc(file_length + 1, sizeof(char));
//    long int num_symb = fread(text, sizeof(char), file_length, sourse);
//
//    text[num_symb] = '\0';
//
//    constructTree(tree, text);
//
//    return 0;
//}
//
//
//static unsigned int MurmurHash2(char* key, unsigned int len) 
//{
//    const unsigned int m = 0x5BD1E995;
//    const unsigned int seed = 0;
//    const int r = 24;
//
//    unsigned int h = seed ^ len;
//
//    const unsigned char* data = (const unsigned char*) key;
//    unsigned int k = 0;
//
//    while (len >= 4) {
//        k  = data[0];
//        k |= data[1] << 8;
//        k |= data[2] << 16;
//        k |= data[3] << 24;
//
//        k *= m;
//        k ^= k >> r;
//        k *= m;
//
//        h *= m;
//        h ^= k;
//
//        data += 4;
//        len -= 4;
//    }
//
//    switch (len) {
//    case 3:
//        h ^= data[2] << 16;
//    case 2:
//        h ^= data[1] << 8;
//    case 1:
//        h ^= data[0];
//        h *= m;
//    };
//    
//    h ^= h >> 13;
//    h *= m;
//    h ^= h >> 15;
//
//    return h;
//}

    // hello vovqa, i love you so fuckin much
