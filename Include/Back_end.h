#ifndef BACK_END_H
    #define BACK_END_H


    struct Name
    {
        char *name; //  !!!! DON"T USE FREEE!!!!!!!!!  will be freed in treeDtor  
        struct
        {
            unsigned is_var   : 1;
            unsigned is_temp  : 1;
            unsigned is_const : 1;
            unsigned is_func  : 1;
        } type;
        union 
        {
            int ram_indx;
            int asm_indx;
        } index;
    };
    
    int generateAsm(Tree *tree, const char *dst_name);


#endif