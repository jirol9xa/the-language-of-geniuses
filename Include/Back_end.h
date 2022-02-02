#ifndef BACK_END_H
    #define BACK_END_H


    struct Name
    {
        char  name[32];          //  !!!! DON"T USE FREEE!!!!!!!!!  will be freed in treeDtor
        char  func_name[32]; // for naming func where var were created  
        struct
        {
            unsigned is_var   : 1;
            unsigned is_temp  : 1;
            unsigned is_const : 1;
            unsigned is_func  : 1;
        } Ntype;
        int ram_indx;
    };

    struct Glob_Name_space
    {
        Stack global_vars;
        Stack func;
    };
    

    int generateAsm(Tree *tree, const char *dst_name);
#endif