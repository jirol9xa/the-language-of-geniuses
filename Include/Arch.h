#ifndef ARCH_H
    #define ARCH_H

    typedef int type_t;


    const int IS_REG = 1 << 5;  //при использовании регистра
    const int IS_RAM = 1 << 6;  //при использовании оперативки
    const int MAX_RAM_SIZE = 1000;
#endif