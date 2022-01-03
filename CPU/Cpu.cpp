#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include "Arch.h"
#include "config.h"
#include "logsLib.h"
#include "textLib.h"
#include "stackLib.h"
#include "../ASM/Asm.h"
#include "Cpu.h"


static int workWithParsedRAM(CPU *cpu, char *code, bool is_pop);


is_debug_lvl_0(
    /*!
        \brief  Функция дампа для проца
        \param  [CPU *]cpu Указатель на процессор
        \param  [char *]binary_code Массив с кодом
    */
    static void cpuDump(CPU *cpu, char *binary_code, Header *header)
    {
        printReshetka();
        printReshetka();
        printReshetka();
        writeLogs("        ");
        for (int i = 0; i < cpu->real_size; i++)
        {
            writeLogs(" %2d", i);
        }
        writeLogs("\n");
        writeLogs("Commands");
        for (int i = 0; i < header->code_length; i++)
        {
            writeLogs(" %2d", (int) binary_code[i]);                                                                                                                              
            if ((binary_code[i] && IS_REG) != 0 || (binary_code[i] && IS_RAM) != 0)       
            {                                   
                i ++;                                                      
                writeLogs(" %2d", *((int *) (binary_code + i)));                            
                i += sizeof(int) - 1;                                                               
            }                                                                                                                                                                                      
        }
        writeLogs("\n");
        writeLogs("%*s----------- current command\n", cpu->real_ip * 3 + 8, "^");

        writeLogs("Regs\n" "ax --- %d\n" "bx --- %d\n" "cx --- %d\n" "dx --- %d\n", cpu->regs[0], 
                cpu->regs[1], cpu->regs[2], cpu->regs[3]);
        
        printReshetka();
        printReshetka();
        printReshetka();
        writeLogs("\n\n\n");
    }
)


int createCpu(CPU *cpu)
{
    CHECK_PTR(cpu);

    stackCtor(&(cpu->stk), 0);
    stackCtor(&(cpu->ret), 0);
    return 0;
}


/*!
    \brief  Функция работы процессора
    \param  [Header *]header Указатель на
            хедер бинарника
    \param  [CPU *]cpu Указатель на процессор
    \return 0 в случае успеха, 1 в противном
            случае
*/
int processing(Header *header, char *code, CPU *cpu)
{
    is_debug_lvl_0(
        CHECK_PTR(header);
        CHECK_PTR(code);
        CHECK_PTR(cpu);
    );

    cpu->ip = 0;
    while (cpu->ip < header->code_length)
    {
        switch (code[cpu->ip ++] & (IS_REG - 1))
        {

            #define DEF_CMD(num, name, num_args, cmd_code)                                                       \
            case CMD_##name:                                                                                     \
                is_debug_lvl_0(cpu->real_ip ++);                                                                 \
                is_debug_lvl_0(cpuDump(cpu, code, header));                                                      \
                cmd_code;                                                                                        \
            break;

            #include "../commands.inc"
            #undef DEF_CMD
        }
    }
    return 0;
}


static int workWithParsedRAM(CPU *cpu, char *code, bool is_pop)
{
    sleep(0.5);
    int num_arguments = code[cpu->ip ++];
    int index = 0;
    if ((num_arguments & IS_REG) != 0 && (num_arguments & IS_RAM) != 0)
    {
        index += cpu->regs[*((int *) (code + cpu->ip))];
        cpu->ip += sizeof(int);
        char sign = code[cpu->ip ++];
        switch(sign)
        {
            case '+':
                index += cpu->regs[*((int *) (code + cpu->ip))];
                break;
            case '-':
                index -= cpu->regs[*((int *) (code + cpu->ip))];
                break;
            case '*':
                index *= cpu->regs[*((int *) (code + cpu->ip))];
                break;
            case '/':
                index /= cpu->regs[*((int *) (code + cpu->ip))];
                break;
        }
        cpu->ip += sizeof(int);
        (is_pop) ? stackPop(&(cpu->stk), &(cpu->RAM[index])) : stackPush(&(cpu->stk), cpu->RAM[index]);
        return 0;
    }
    else if ((num_arguments & IS_REG) != 0 && (num_arguments & (IS_REG - 1)) == 2)
    {
        index += cpu->regs[*((int *) (code + cpu->ip))];
        cpu->ip += sizeof(int);
        char sign = code[cpu->ip ++];
        switch(sign)
        {
            case '+':
                index += *((int *) (code + cpu->ip));
                break;
            case '-':
                index -= *((int *) (code + cpu->ip));
                break;
            case '*':
                index *= *((int *) (code + cpu->ip));
                break;
            case '/':
                index /= *((int *) (code + cpu->ip));
                break;
        }
        cpu->ip += sizeof(int);
        (is_pop) ? stackPop(&(cpu->stk), &(cpu->RAM[index])) : stackPush(&(cpu->stk), cpu->RAM[index]);
        return 0;
    }
    else if (((num_arguments & IS_RAM) != 0) && (num_arguments & (IS_REG - 1)) == 2)
    {
        index += *((int *) (code + cpu->ip));
        cpu->ip += sizeof(int);
        char sign = code[cpu->ip ++];
        switch(sign)
        {
            case '+':
                index += cpu->regs[*((int *) (code + cpu->ip))];
                break;
            case '-':
                index -= cpu->regs[*((int *) (code + cpu->ip))];
                break;
            case '*':
                index *= cpu->regs[*((int *) (code + cpu->ip))];
                break;
            case '/':
                index /= cpu->regs[*((int *) (code + cpu->ip))];
                break;
        }
        cpu->ip += sizeof(int);
        (is_pop) ? stackPop(&(cpu->stk), &(cpu->RAM[index])) : stackPush(&(cpu->stk), cpu->RAM[index]);
        return 0;
    }
    else if ((num_arguments & (IS_REG - 1)) == 2)
    {
        index += *((int *) (code + cpu->ip));
        cpu->ip += sizeof(int);
        char sign = code[cpu->ip ++];
        switch(sign)
        {
            case '+':
                index += *((int *) (code + cpu->ip));
                break;
            case '-':
                index -= *((int *) (code + cpu->ip));
                break;
            case '*':
                index *= *((int *) (code + cpu->ip));
                break;
            case '/':
                index /= *((int *) (code + cpu->ip));
                break;
        }
        cpu->ip += sizeof(int);
        (is_pop) ? stackPop(&(cpu->stk), &(cpu->RAM[index])) : stackPush(&(cpu->stk), cpu->RAM[index]);
        return 0;
    }
    else if ((num_arguments & (IS_REG - 1)) == 1 && (num_arguments & IS_REG) != 0)
    {
        index += cpu->regs[*((int *) (code + cpu->ip))];
        cpu->ip += sizeof(int);
        (is_pop) ? stackPop(&(cpu->stk), &(cpu->RAM[index])) : stackPush(&(cpu->stk), cpu->RAM[index]);
        return 0;
    }

    return -1;
}

/*
void closeCPULogs()
{
    closeLogs();
}
*/