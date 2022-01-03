#ifndef ASM.H
    #define ASM.h

    struct Label
    {
        char name[32];
        int  ip_number;
    };

    struct Label_array
    {
        Label *label;
        int  labeles_amount;
        int capacity;
    };


    #if DEBUG_LVL > 0
        int list(char *CODE, void* arg, int code, int is_ram);
        int print_err_asm(int err);

        enum asm_errors
        {
            ERRASM_NO_HLT = 1
        };

    #endif

    int getLine(char *dest, char *sourse);

    int parseOperand(char *regsRAM, Header *header, char *code, int num);

    int getLabeles(Text *command, Label_array *lables);
    int writeLabel(char *binary_code, Label_array *lables, char *label_name, Header *header, int CMD_TYPEJUMP);
    int writeCall(char *func_name, Label_array *marks, Header *header, char *binary_code);

#endif