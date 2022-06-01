# My programming language

## Basic rules
-------------------------------------------------
The syntactic rules of my language are similar to the rules of writing in the C programming language. 
The main difference is that the word "define" is used to define the function. After the code, you need to put "$" at the end of the text file.

</br>

## Changeable language
-------------------------------------------------
All commands that the language supports are listed in the text file "cmd_file.txt ", which is located in the main folder of the project. Due to the implementation of keyword search using a suffix tree, it is possible to change the syntax of the language directly in this file, without recompiling. (it is necessary to change the words that are enclosed between ->$ and $). Please note that only lowercase English words are supported

</br>

## Code examples
-------------------------------------------------
**Factorial:**

    define fctr(n)
    {
       if (n == 1)
       {
           return 1;
       }
       a = n - 1;
       p = fctr(a);
       return p * n;
    }
    define main()
    {
        a = 0;
        scanf(a);
        b = fctr(a);
        printf(b);
    }$

</br>

**The quadratic equation:**

    define main()
    {
        a = 0;
        scanf(a);
        b = 0;
        scanf(b);
        c = 0;
        scanf(c);
        x = 0;
        y = 0;
        d = 0;
        s = 0;    
        if (a == 0)
        {
            if (b == 0)
            {
                printf(3);
            }
            if (b != 0)
            {
                x = (0 - c) / b;
                printf(x);
            }
        }
        if (a != 0)
        {
            d = b * b - 4 * a * c;
            if (d < 0)
            {
                printf(0);
            }
            if (d > 0)
            {
                s = sqrt(d);
                x = (s - b) / (2 * a);
                printf(x);
                y = (0 - s - b) / (2 * a);
                printf(y);
            }
        }

        return 0;
    }$

</br>

## A little about the history of the project
-------------------------------------------------
All files of this project were written in December 2021, some in February 2022. In the future, some of them were slightly added so that everything worked correctly. At the handover, my supervisor and I saw how many things can be corrected in the old code, but the supervisor did not consider it necessary to correct the design of the code of the training task, which was six months ago.

Currently, a preprocessor has been written for the language, which allowed us to radically change the syntax: add Russian words and make commands verbose, but it's better not to know about it (trust me).

</br>

## Installation 

    git clone https://github.com/jirol9xa/the-language-of-geniuses

making project: 

    cd run && make

running: 

    ./lang <code_file>