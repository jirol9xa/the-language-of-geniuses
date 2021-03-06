#ifndef SYNTAX_H
    #define SYNTAX_H

    // TODO (if i will stay alive)
    //const char *KEYWORDS[] = {
    //    "Живой?"             //   if
    //    "Пока еще не сессия" //   while
    //    "Перевестись на РТ"  //   return
    //    "Поступить на ФТ"    //   define
    //    "Дед сказал"         //   const
    //    "Пойти за пивом"     //   (
    //    "Купить водки"       //   )
    //    ""
    //};

    // hello vovqa, i love you so fuckin much

    enum KEY_TYPES
    {
        IS_IF        =  1,
        IS_WHILE     = (1 << 1),
        IS_RETURN    = (1 << 1) + 1,
        IS_CONST     = (1 << 2),
        IS_MAIN      = (1 << 2) + 1,
        IS_DEFINE    = (1 << 2) + (1 << 1),
        IS_STATEMENT = (1 << 2) + (1 << 1) + 1,
        IS_FUNCTION  = (1 << 3),
        IS_CALL      = (1 << 3) + 1,
        IS_DECISION  = (1 << 3) + (1 << 1),
        IS_PARAM     = (1 << 3) + (1 << 1) + 1,
        IS_ELSE      = (1 << 3) + (1 << 2),
        IS_PRINTF    = (1 << 3) + (1 << 2) + 1,
        IS_SCANF     = (1 << 3) + (1 << 2) + (1 << 1),

        IS_SIN       = (1 << 3) + (1 << 1),
        IS_COS       = (1 << 3) + (1 << 1) + 1,
        IS_LN        = (1 << 3) + (1 << 2), 
        IS_SQRT      = (1 << 3) + (1 << 2) + 1,    
    };


    int SyntaxError  ();


    #define SYNTAX_ERR             \
    {                              \
        PRINT_RESHETKA;            \
        printf("Syntax err in\n"); \
        PRINT_RESHETKA;            \
        PRINT_LINE;                \
        SyntaxError();             \
    }

    #define PRINT_RESHETKA printf("#######################################\n");


#endif