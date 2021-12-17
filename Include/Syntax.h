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
        IS_IF     = 1,
        IS_WHILE  = 1 << 2,
        IS_RETURN = 1 << 2 + 1,
        IS_CONST  = 1 << 3,
        IS_MAIN   = 1 << 3 + 1,
        IS_DEFINE = 1 << 3 + 1 << 2,
        IS_STATEMENT = 1 << 4 + 1,

        IS_SIN    = 1 << 6,
        IS_COS    = 1 << 7,
        IS_LN     = 1 << 7 + 1,     
    };

#endif