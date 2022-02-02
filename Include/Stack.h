#ifndef STACK_H
    #define STACK_H

    #define type Name*
    #define DEBUG_LVL 0
    //const type POISON = 0xDEAD; 

    /*
                        +--> code, при DEBUG_LVL > 1
                        |
    is_debug(code) -----+
                        |
                        +--> `пустота`, при !(DEBUG_LVL > 1)
    */

    #if DEBUG_LVL > 0
        #define is_debug_lvl_0(code) code
    #else
        #define is_debug_lvl_0(code) ;
    #endif

    #if DEBUG_LVL > 1
        #define is_debug_lvl_1(code) code
    #else
        #define is_debug_lvl_1(code) ;
    #endif

    #ifndef DEBUG
        #define DEBUG

        #define FUNC_REPORT(func, stack)  func;

        enum Error {
                OK = 0, 
                ERR_CALLING_FUNC_FAILED        =         1,
                ERR_STACK_ALREARY_CREATED      =    1 << 1,
                ERR_EMPTY_ELEM_ISNT_POISONED   =    1 << 2,
                ERR_SIZE_GREATER_CAPACITY      =    1 << 3,
                ERR_POP_EMPTY_STACK            =    1 << 4,
                ERR_RIGHT_CANARY_DAMAGED       =    1 << 5,
                ERR_LEFT_CANARY_DAMAGED        =    1 << 6,
                ERR_WRONG_HASH                 =    1 << 7,
                ERR_INVALID_PTR                =    1 << 8,
                ERR_REALLOC_FAILED             =    1 << 9,
                ERR_PUSH_FAILED                =    1 << 10,
                ERR_STACK_ALREADY_CLEANED      =    1 << 11,
                ERR_LEFT_DATACANARY_DAMAGED    =    1 << 12,
                ERR_RIGHT_DATACANARY_DAMAGED   =    1 << 13,
                ERR_RESIZE_FAILED              =    1 << 14,
                ERR_POP_FAILED                 =    1 << 15,
                ERR_DUMP_FAILED                =    1 << 16,
                ERR_STACK_BROKEN               =    1 << 17
            };

        #if (DEBUG_LVL > 0)
            #define STACK_CREATION_INFO(stack) {                                                                  \
                fprintf(logs, "Stack \"%s\" was created in function \"%s\" on line %d."                           \
                "Adress %p with addrreess of data: %p\n\n\n", #stack, __PRETTY_FUNCTION__, __LINE__,              \
                stack, stack->data);                                                                              \
            }

            #define STACK_DUMP(stack) {                                                                           \
                verifyStack(stack);                                                                               \
                stackDump(stack, __func__, #stack);                                                               \
            }   


            #define CHECK_PTR(arg) {                                                                              \
                if (!(arg)) {                                                                                     \
                    fprintf(logs, "[%s:%d] --- %s failed\n\n\n", __func__, __LINE__, #arg);                       \
                    return ERR_INVALID_PTR;                                                                       \
                }                                                                                                 \
            }

            #define ASSERT_OK(stack) {                                                                            \
                if (stack->status) return ERR_STACK_BROKEN;                                                       \
            }

            #define FUNC_REPORT(func, stack) {                                                                    \
                int status = (int) func;                                                                          \
                if (status) {                                                                                     \
                    fprintf(logs, "########################################################################\n"    \
                                "function \"%s\" was failed with error code:\n", #func);                          \
                    printError(status);                                                                           \
                    fprintf(logs, "\nStack is:\n");                                                               \
                    STACK_DUMP(stack)                                                                             \
                    fprintf(logs, "########################################################################\n\n");\
                }                                                                                                 \
            }

            #define CANARY(stack) (0xFEE1DEAD^((u_int64_t) stack))

            #define LEFT_CANARY(stack) ((char*) stack->data - sizeof(u_int64_t))

            #define RIGHT_CANARY(stack) ((char*) stack->data + stack->capacity * sizeof(type))
        #endif

    #endif

    struct Stack
    {

        is_debug_lvl_1(u_int64_t egg);
        type* data;
        int size;
        int capacity;
        is_debug_lvl_0(
            int status;
            is_debug_lvl_1(u_int64_t hash);
            is_debug_lvl_1(u_int64_t chicken);  
        ) 
    };

    /*!
        \brief  Функция создания стека
        \param  [Stack*]stk Указатель на стек
        \param  [int]capacity Емкость стека
        \return 0 или код ошибки
    */
    int stackCtor(Stack* stk, int capacity);

    /*!
        \brief  Функция уничтожения стека
        \param  [Stack*]stk Указатель на стек
        \return 0 или код ошибки
    */
    int stackDtor(Stack* stk);

    /*!
        \brief  Функция добавления элемента в стек
        \param  [Stack*]stk Указатель на стек
        \param  [type]value Значение добавляемого
                элемента
        \return 0 или код ошибки
    */
    int stackPush(Stack* stk, type value);

    /*!
        \brief  Функция извлечения элемента
        \param  [Stack*]stk Указатель на стек
        \param  [type*]param Указатель, куда
                должен быть извлечен элемент
        \return 0 или код ошибки
    */
    int stackPop(Stack* stk, type* param);

    /*!
        \brief  Функция вывода информации о
                стеке и функции, которая его
                вызвала
    */
    void stackDump(const Stack* stk, const char* func_name, const char* stack_name);

    /*!
        \brief  Функция проверки стека на
                корректность
    */
    void verifyStack(Stack* stk);

    /*!
        \brief  Функция печати ошибок
    */
    void printError(int Error);

#endif
