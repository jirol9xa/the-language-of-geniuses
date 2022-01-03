#ifndef CPU_H
    #define CPU_H


    struct CPU {
        int regs[4];    //ax, bx, cx, dx
        Stack stk;
        int ip;
        int RAM[1000];
        //int code_size;
        Stack ret;
        is_debug_lvl_0(
        int real_size;
        int real_ip;
        )
    };


    int processing(Header *header, char *code, CPU *cpu);
    int createCpu(CPU *cpu);

#endif

