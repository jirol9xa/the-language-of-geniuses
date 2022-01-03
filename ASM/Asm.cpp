#include "Arch.h"
#include "config.h"
#include <stdio.h>
#include <cmath>
#include <string.h>
#include <cstring>
#include <ctype.h>

#if DEBUG_LVL > 0
    extern FILE *listing;
#endif


#include "logsLib.h"
#include "textLib.h"
#include "stackLib.h"
#include "Asm.h"



static int parsingForRAM(char *regsRAM, Header *header, char *binary_code, int num);
static int Atoi(char *number, int size);
static int resizeLabelArr(Label_array *lables);
static void writeInt(char *binary_code, Header *header, int value);



static int Atoi(char *number, int size = 0)
{
    int num = 0;
    if (size == 0) size = strlen(number);

    for (int i = 0; i < size && number[i] != '\0'; i++)
    {
        num += (number[i] - 48) * pow(10, size - 1 - i);
    }

    return num;
}


static int resizeLabelArr(Label_array *labels)
{
    void *temp_ptr = nullptr;
    temp_ptr = realloc(labels->label, sizeof(Label) * labels->capacity * 2);
    if (temp_ptr != nullptr)
    {
        labels->label = (Label *) temp_ptr;
        labels->capacity *= 2;
        return 0;
    }

    writeLogs("!!! ERROR Not enough memory for new label !!!\n");
    return -1;
}


is_debug_lvl_0(
    /*!
        \brief  Функция печати листинга
        \param  [char *]CODE массив с кодом
        \param  [int]arg аргумент
        \param  [int]code Код команды
        \return 0, если все успешно, 1
                в противном случае
    */
    int list(char *CODE, void* arg, int code, int IS_RAM)
    {
        CHECK_PTR(CODE);

        if (!IS_RAM)
        {
        int *Arg = (int *) arg;
        fprintf(listing, "%4s %10d    ------------->  %4d %10d\n", CODE, *Arg, code, *Arg);
        return 0;
        }
        else
        {
            char *Arg = (char *) arg;
            if (Arg[0] == '[')
            {
                int adr = Atoi(Arg + 1, strlen(Arg) - 2);
                fprintf(listing, "%4s %10s    ------------->  %4d %10d\n", CODE, Arg, code, adr);
            }
            else
            {   
                fprintf(listing, "%4s %10s    ------------->  %4d %10d\n", CODE, Arg, code, Arg[0] - 96);   
            }
            return 0;
        }
    }


    /*!
        \brief  Функция печати ошибки
        \param  [int]err Код ошибки
    */
    int print_err_asm(int err)
    {
        switch (err)
        {
            case ERRASM_NO_HLT:
                writeLogs("ERRASM_NO_HLT\n");
                break;
        }
    }
)


/*!
    \brief  Функция считывания строки
    \param  [char *]arg Строка, 
*/
int getLine(char *dest, char *sourse)
{
    int i = 0, j = 0;

    while (sourse[j] == ' ') 
    {
        j ++;
        continue;
    }
    for (; sourse[i + j] != '\n' && sourse[i + j] != '\0'; i++)
    {
        dest[i] = sourse[i + j];
    }

    return i;
}


int parseOperand(char *regsRAM, Header *header, char *code, int num)
{
    if (regsRAM[0] == '[')
    {
        parsingForRAM(regsRAM + 1, header, code, num);
    }
    else{
        code[header->code_length++] = (num | IS_REG);
        header->real_length ++;
        writeInt(code, header, regsRAM[0] - 'a');
    }
    
    return 0;
}


static int parsingForRAM(char *regsRAM, Header *header, char *binary_code, int cmd_num)
{
    char sign      = 0;
    int  num       = 0;
    int second_num = 0;
    int status = sscanf(regsRAM, "%d %c %d", &num, &sign, &second_num) - (sign == 93);
    
    if (status == 0)
    {
        char reg[32] = {};
        status = sscanf(regsRAM, "%s %c %d", reg, &sign, &num) - (sign == 93);
        if (status == 3) //reg and num
        {
            binary_code[header->code_length ++] = (cmd_num | IS_REG | IS_RAM);
            header->real_length ++;
            binary_code[header->code_length ++] = 2 | IS_REG;   //IS_REG if first arg is register
            header->real_length ++;

            writeInt(binary_code, header, reg[0] - 'a');

            binary_code[header->code_length ++] = sign;
            header->real_length ++;

            writeInt(binary_code, header, num);
        }
        else if (status == 2) //reg and reg
        {
            char second_reg[32] = {};
            status = sscanf(regsRAM, "%s %c %s", reg, &sign, second_reg) - (sign == 93);


            binary_code[header->code_length ++] = (cmd_num | IS_REG | IS_RAM);
            header->real_length ++;
            binary_code[header->code_length ++] = 2 | IS_REG | IS_RAM;  //IS_REG and IS_RAM if both args are registers
            header->real_length ++;

            writeInt(binary_code, header, reg[0] - 'a');

            binary_code[header->code_length ++] = sign;
            header->real_length ++;

            writeInt(binary_code, header, second_reg[0] - 'a');
        }
        else if (status == 1)  //reg
        {
            binary_code[header->code_length ++] = (cmd_num | IS_REG | IS_RAM);
            header->real_length ++;
            binary_code[header->code_length ++] = 1 | IS_REG;   //is_reg if first arg is register
            header->real_length ++;

            writeInt(binary_code, header, reg[0] - 'a');
        }
    }
    else if (status == 1)   //num
    {
        binary_code[header->code_length ++] = (cmd_num | IS_RAM);
        header->real_length ++;
        
        writeInt(binary_code, header, num);

    }
    else if (status == 2)  //num and reg
    {
        char reg[32] = {};
        sscanf(regsRAM, "%d %c %s", &num, &sign, reg);

        binary_code[header->code_length ++] = (cmd_num | IS_REG | IS_RAM);
        header->real_length ++;
        binary_code[header->code_length ++] = 2 | IS_RAM;   //is_ram if second is register
        header->real_length ++;

        writeInt(binary_code, header, num);

        binary_code[header->code_length ++] = sign;
        header->real_length ++;

        writeInt(binary_code, header, reg[0] - 'a');
    
    }
    else  //num and num
    {
        binary_code[header->code_length ++] = (cmd_num | IS_REG | IS_RAM);
        header->real_length ++;
        binary_code[header->code_length ++] = 2;
        header->real_length ++;

        writeInt(binary_code, header, num);


        binary_code[header->code_length ++] = sign;
        header->real_length ++;

        writeInt(binary_code, header, second_num);

    }
}



int getLabeles(Text *command, Label_array *marks)
{
    int getMarks_ip = 0;

    for (int i = 0; i < command->string_amount; i++)
    {
        bool is_call_or_ret = false;

        if (strncmp(command->text[i].string, ":CALL", strlen(":CALL")) == 0)
        {
            is_call_or_ret = true;
            getMarks_ip += 1 + sizeof(int);
        }
        else if (strncmp(command->text[i].string, ":RET", strlen(":RET")) == 0)
        {
            is_call_or_ret = true;
            getMarks_ip ++;
        }
        else
        {
            #define DEF_CMD(num, name, num_arg, code)                                                                       \
            if (strncmp(#name, command->text[i].string, strlen(#name)) == 0)                                                \
            {                                                                                                               \
                if (num_arg > 0)                                                                                            \
                {                                                                                                           \
                    getMarks_ip += 1 + sizeof(type_t);                                                                         \
                }                                                                                                           \
                else                                                                                                        \
                {                                                                                                           \
                    getMarks_ip ++;                                                                                         \
                }                                                                                                           \
            }
            
            #include "../commands.inc"
            #undef DEF_CMD
        }
        if (command->text[i].string[0] == ':' && !is_call_or_ret)
        {
            if (marks->labeles_amount + 1 > marks->capacity)
            {
                int status = resizeLabelArr(marks);
                if (status)
                {
                    printf("%s ----- getMarks ERROR\n", command->text[i].string + 1);
                    return 1;
                }
            }

            int sscanf_status = sscanf(command->text[i].string + 1, "%s", marks->label[marks->labeles_amount].name);
            if (sscanf_status)
            {
                marks->label[marks->labeles_amount++].ip_number = getMarks_ip;
            }
            else
            {
                printf("%s ----- getMarks ERROR\n", command->text[i].string + 1);
                return 1;
            }
        }
    }

    return 0;
}


int writeLabel(char *binary_code, Label_array *marks, char *mark_name, Header *header, int CMD_TYPEJUMP)
{   
    for (int i = 0; i < marks->labeles_amount; i++)
    {
        if (strcmp(marks->label[i].name, mark_name) == 0)
        {
            binary_code[header->code_length ++] = CMD_TYPEJUMP;
            header->real_length ++;
            writeInt(binary_code, header, marks->label[i].ip_number);
            return 0;
        }
    }

    writeLogs("!!! ERROR INVALID LABLE !!!\n");
}


int writeCall(char *func_name, Label_array *marks, Header *header, char *binary_code)
{
    for (int i = 0; i < marks->labeles_amount; i++)
    {
        if (strcmp(marks->label[i].name, func_name) == 0)
        {
            writeInt(binary_code, header, marks->label[i].ip_number);
            return 0;
        }
    }

    writeLogs("!!! ERROR INVALID LABLE !!!\n");
}


static void writeInt(char *binary_code, Header *header, int value)
{
    *((int *) (binary_code + header->code_length)) = value;
    header->code_length += sizeof(int);
    header->real_length ++;
}
