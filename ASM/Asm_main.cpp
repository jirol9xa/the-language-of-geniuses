#include "logsLib.h"
#include "Arch.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <cmath>


#if DEBUG_LVL > 0
    FILE *listing = fopen("Listing", "w"); //не глобальная
#endif

#include "textLib.h"
#include "stackLib.h"
#include "../ASM/Asm.h"


int main(int argc, const char **argv)
{   
    is_debug_lvl_0(
        if (openLogs())
        {
            printf("!!!ASM finished !!!\n");
            return 0;
        }
    );
    
    if (argc < 2)
    {
        is_debug_lvl_0(writeLogs("!!! ERROR Can't run asm without sourse file !!!\n"));
        return 0;
    }
    
    FILE *sourse = fopen(argv[1], "r");
    FILE *binary = fopen("Binary", "wb");

    is_debug_lvl_0(
        CHECK_PTR(sourse);
        CHECK_PTR(binary);
    )

    Text commands = {};

    constructText(sourse, &commands);
    char *binary_code = (char *) calloc((commands.string_amount + 1) * 2, sizeof(type_t));
    is_debug_lvl_0(CHECK_PTR(binary_code));
    
    Header header = {};
    header.sign = 'HM';
    header.ver = 0;
    header.hash = 0;

    Label_array marks = {};
    marks.label = (Label *) calloc(1, sizeof(Label));
    marks.capacity = 1;
    getLabeles(&commands, &marks);
    header.code_length += sizeof(Header);
    
    char CMD[32] = {};

    for (int i = 0; i < commands.string_amount; i++)
    {   
        int arg = 0;
        char str_buff[32] = {};
        int arg_amount = sscanf(commands.text[i].string, "%s %d", CMD, &arg) - 1;

        if (arg_amount < 0)
        {
            is_debug_lvl_0(
                printReshetka();
                writeLogs("not valid format for command\n");
                printReshetka();
            )
        }
        else if (arg_amount == 0)
        {
            if (getLine(str_buff, commands.text[i].string + strlen(CMD)) > 0)
            {
                arg_amount = -1;
            }
        }

        if (CMD[0] == ':')
        {
            if (strncmp(CMD, ":CALL", strlen(":CALL")) == 0)
            {
                char func_name[32] = {};
                int status = sscanf(commands.text[i].string + strlen(CMD), "%s", func_name);
                if (status)
                {
                    binary_code[header.code_length ++] = CMD_CALL;
                    header.real_length ++;
                    writeCall(func_name, &marks, &header, binary_code);
                }
                else
                {
                    writeLogs("!!! Write CALL ERROR !!!\n");
                }
            }
            else if (strncmp(CMD, ":RET", strlen(":RET")) == 0)
            {
                binary_code[header.code_length ++] = CMD_RET;
                header.real_length ++;
            }

            continue;
        }
        #define DEF_CMD(num, name, num_arg, code)                                                           \
        if (strcmp(CMD, #name) == 0)                                                                        \
        {                                                                                                   \
            if (num_arg == 1)                                                                               \
            {                                                                                               \
                if (arg_amount == 1)                                                                        \
                {                                                                                           \
                    binary_code[header.code_length ++] = num;  /*если обычный пуш*/                         \
                    header.real_length ++;                                                                  \
                    *((int *) (binary_code + header.code_length)) = arg;                                    \
                    is_debug_lvl_0(list(CMD, &arg, binary_code[header.code_length - 1], 0));                \
                    header.code_length += sizeof(int);                                                      \
                    header.real_length ++;                                                                  \
                }                                                                                           \
                else if (num >= CMD_JMP && num <= CMD_JNE)                                                  \
                {                                                                                           \
                    char mark_name[32] = {};                                                                \
                    sscanf(commands.text[i].string + strlen(CMD), "%s", mark_name);                         \
                    writeLabel(binary_code, &marks, mark_name, &header, num);                               \
                }                                                                                           \
                else if (arg_amount == -1)                                                                  \
                {                                                                                           \
                    parseOperand(str_buff, &(header), binary_code, num);                                    \
                    is_debug_lvl_0(list(CMD, str_buff,                                                      \
                                   (int) *(binary_code + header.code_length - sizeof(int) - 1), 1));        \
                }                                                                                           \
            }                                                                                               \
            else                                                                                            \
            {                                                                                               \
                binary_code[header.code_length ++] = CMD_##name;                                            \
                header.real_length ++;                                                                      \
                is_debug_lvl_0(list(CMD, &arg, binary_code[header.code_length - 1], 0));                    \
            }                                                                                               \
        }                                                                                                   \
        else   

        #include "../commands.inc"

        /*else*/
        {
            printf("ERROR %s\n", CMD);
        }

        #undef DEF_CMD
    }

    binary_code[header.code_length ++] = CMD_HLT;
    header.real_length ++;

    header.code_length -= sizeof(Header);
    *((Header *) binary_code) = header;

    fwrite(binary_code, sizeof(char) * (header.code_length + sizeof(Header)), 1, binary);

    free(marks.label);
    free(binary_code);
    fclose(sourse);
    fclose(binary);
    finish_text(&commands);
    is_debug_lvl_0(
        fclose(listing);
        closeLogs();
    )
    return 0;
}