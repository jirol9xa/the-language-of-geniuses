#include "stdio.h"
#include "assert.h"
#include "stdlib.h"
#include "cmath"
#include "ctype.h"
#include "string.h"
#include "Tree.h"
#include "Back_end.h"
#include "Stack.h"

//extern FILE* logs;


/*!
    \brief  Функция изменения емкости стека
    \param  [Stack*]stk Указатель на стек
    \param  [int]upper 1, если требуется
            увеличить емкость, 0 -- если
            нужно уменьшить емкость
    \return 0 или код ошибки
*/
static int stackResize(Stack* stk, int upper);

/*!
    \brief  Функция копирования памяти
    \param  [void*]destination Место,
            куда надо записать данные
    \param  [void*]sourse Место, откуда
            надо взять данные
    \param  [int]element_siez размер
            элемента
    \return 0 или код ошибки
*/
static int MemCpy(void* destination, void* sourse, int element_size);

/*!
    \brief  Функция подсчета хеша
    \param  [Stack*]stk Указатель на стек
    \return 0 или код ошибки
*/
static unsigned int hashCalc(Stack* stk);

/*!
    \brief  Функция посчет хеша по 
            алгоритму MurmurHash2
*/
static unsigned int MurmurHash2(char* key, unsigned int len);

/*!
    \brief  Функция установки левой или
            правой канарейки для data
    \param  [Stack*]stk Указатель на стек
    \param  [int]is_left 1, если надо 
            установить левую канарейку,
            !1 -- если правую
*/
static void setCanary(Stack* stk, int is_left);


int stackCtor(Stack* stk, int capacity) 
{   
    is_debug_lvl_0(CHECK_PTR(stk));

    stk->capacity = (int) pow(2, (int) (log(capacity) / log(2)) + 1 );
    stk->data     = (type*) calloc (1 , sizeof(type) * stk->capacity);

    is_debug_lvl_1(
        void* temp_ptr = realloc(stk->data, sizeof(type) * stk->capacity + 2 * sizeof(u_int64_t));
        if (temp_ptr == nullptr) {
            return ERR_REALLOC_FAILED;
        }

        stk->data = (type*) ((char*) temp_ptr + sizeof(u_int64_t));
        stk->egg     = CANARY(stk);
        stk->chicken = CANARY(stk);

        setCanary(stk, 1);
        setCanary(stk, 0);
    )

    //for (int i = 0; i < stk->capacity; i++) {
    //    stk->data[i] = POISON;
    //}

    is_debug_lvl_1( if (hashCalc(stk) == ERR_INVALID_PTR) return ERR_INVALID_PTR);

    is_debug_lvl_0(
        STACK_CREATION_INFO(stk)
        STACK_DUMP(stk)
    )

    return 0;
}


int stackDtor(Stack* stk) 
{
    is_debug_lvl_0(
        CHECK_PTR(stk)
        STACK_DUMP(stk)
        ASSERT_OK(stk)
    )

    stk->capacity = 0;
    stk->size = 0;

    is_debug_lvl_1(free(LEFT_CANARY(stk)));

    #if DEBUG_LVL <= 1 
        free(stk->data);
    #endif
    stk->data = (type*) 0xBEBE;
    return 0;
}


int stackPush(Stack* stk, type value) 
{
    is_debug_lvl_0(
        CHECK_PTR(stk)
        STACK_DUMP(stk)
        ASSERT_OK(stk)
    )

    if (stk->size + 1 > stk->capacity) {
        if (stackResize(stk, 1) == ERR_RESIZE_FAILED) {
            is_debug_lvl_0(
            fprintf(logs, "Error in %s on %d line: not enough memory for stack\n\n\n", __FUNCTION__, __LINE__);
            )

            return ERR_PUSH_FAILED;
        }
    }

    stk->size ++;

    MemCpy((char*) &stk->data[stk->size - 1], &value, sizeof(type));

    is_debug_lvl_1(
        if (hashCalc(stk) == ERR_INVALID_PTR) return ERR_PUSH_FAILED;
    )

    is_debug_lvl_0(
        STACK_DUMP(stk);
    )
        
    return 0;
}


static int stackResize(Stack* stk, int upper) 
{
    is_debug_lvl_0(CHECK_PTR(stk)
                   STACK_DUMP(stk)
                   ASSERT_OK(stk)
    )

    //stk->capacity = stk->capacity * 2 + !stk->capacity;

    if (upper) {
        void* temp_ptr = nullptr;

        temp_ptr = realloc(stk->data, sizeof(type) * 2 * stk->capacity);
        is_debug_lvl_1(temp_ptr = realloc(LEFT_CANARY(stk), sizeof(type) * 2 * stk->capacity  + sizeof(u_int64_t) * 2));

        if (temp_ptr != nullptr) {
            stk->capacity *= 2;
            stk->data = (type*) ((char*) temp_ptr + sizeof(u_int64_t));

            is_debug_lvl_1(
                setCanary(stk, 0);
            )
                
            //for (int i = stk->size; i < stk->capacity; i++) {
            //    stk->data[i] = POISON;
            //}

            is_debug_lvl_1(
                if (hashCalc(stk) == ERR_INVALID_PTR) return ERR_INVALID_PTR;
            )
            is_debug_lvl_0(STACK_DUMP(stk));

            return 0;
        }
        else {
            is_debug_lvl_0(STACK_DUMP(stk));
            
            return ERR_RESIZE_FAILED;
        }
    }
    else {
        is_debug_lvl_1(
            realloc(LEFT_CANARY(stk), sizeof(type) * (stk->capacity / 2) + 2 * sizeof(u_int64_t));
            setCanary(stk, 0);
            hashCalc(stk);
        )

        #if DEBUG_LVL <= 1
            stk->data = (Name *) realloc(stk->data, sizeof(type) * (stk->capacity / 2));
        #endif

        stk->capacity /= 2;

        return 0;
    }
}


int stackPop(Stack* stk, type* param)
{   
    
    is_debug_lvl_0(
        STACK_DUMP(stk)
        ASSERT_OK(stk)
    )

    stk->size--;
    MemCpy(param, &(stk->data[stk->size]), sizeof(type));
    //stk->data[stk->size] = POISON;

    while (stk->capacity / 2 - stk->size > 3) {
        is_debug_lvl_1(hashCalc(stk);)
        FUNC_REPORT(stackResize(stk, 0), stk)
    }

    is_debug_lvl_1(hashCalc(stk));

    is_debug_lvl_0(STACK_DUMP(stk));

    return 0;
}


is_debug_lvl_0(
    void verifyStack(Stack* stk){
        if (stk->data == (type*) 0xBEBE) {
            stk->status |= ERR_STACK_ALREADY_CLEANED;
        }
        if (stk->status & (ERR_STACK_ALREADY_CLEANED) == 0){
            is_debug_lvl_1(
                if (stk->hash != hashCalc(stk)) {
                    stk->status |= ERR_WRONG_HASH;
                }
            )                                                                                                 
            if (stk->size > stk->capacity) {  
                stk->status |= ERR_SIZE_GREATER_CAPACITY;                       
            }
            is_debug_lvl_1(
                if (stk->egg != CANARY(stk)) {
                    printf("stk->egg ---- %lu\n", stk->egg);
                    stk->status |= ERR_LEFT_CANARY_DAMAGED;
                }
                if (stk->chicken != CANARY(stk)) {
                    stk->status |= ERR_RIGHT_CANARY_DAMAGED;
                }
                if ( (*((u_int64_t*) (LEFT_CANARY(stk)) )) != CANARY(stk)){
                    stk->status |= ERR_LEFT_DATACANARY_DAMAGED;
                }
                if ( (*((u_int64_t*) (RIGHT_CANARY(stk)))) != CANARY(stk)) {
                    stk->status |= ERR_RIGHT_DATACANARY_DAMAGED;
                }
            )
            for (int i = stk->size; i < stk->capacity; i++){
                if (stk->data[i] != POISON){
                    stk->status |= ERR_EMPTY_ELEM_ISNT_POISONED;
                }
            }
        }

    }


    void stackDump(const Stack* stk, const char* func_name, const char* stack_name) {
        fprintf(logs, "Stack <> adress[%p] \"%s\" at %s\n", stk, stack_name, func_name);
        fprintf(logs, "---------------------------------------------------------------------------------\n");
        fprintf(logs, "STATUS = %16s\n" "stack size = %12d\n" "stack capacity = %8d\n" 
                "data [%p]\n", (stk->status == OK) ? "OK" : "BROKEN", stk->size, stk->capacity, stk->data);

        if (stk->status){
            printError(stk->status);
            fprintf(logs, "\n");
        }
        else {
            for (int i = 0; i < stk->size; i++){
                fprintf(logs, "*data[%d] = %d\n", i, stk->data[i]);
            }
            for (int i = stk->size; i < stk->capacity; i++){
                fprintf(logs, "*data[%d] = %X\n", i, stk->data[i]);
            }
        }

        fprintf(logs, "---------------------------------------------------------------------------------\n\n\n");
        fflush(logs);
    }   

)


static int MemCpy(void* destination, void* sourse, int element_size)
{
    #if DEBUG_LVL > 0
        CHECK_PTR(destination)
        CHECK_PTR(sourse)
    #endif

    char* dest = (char*) destination;
    char* srs = (char*) sourse;

    while (element_size >= 16) {
        *((__int128_t*) dest) = *((__int128_t*) srs);

        dest += 16;
        srs += 16;
        element_size -= 16;
    }
    while (element_size >= 8) {
        *((__int64_t*) dest) = *((__int64_t*) srs);

        dest += 8;
        srs += 8;
        element_size -= 8;
    }
    while (element_size >= 4) {
        *((__int32_t*) dest) = *((__int32_t*) srs);

        dest += 4;
        srs += 4;
        element_size -= 4;
    }
    while (element_size >= 2) {
        *((__int16_t*) dest) = *((__int16_t*) srs);

        dest += 2;
        srs += 2;
        element_size -=2;
    }
    if (element_size == 1) {
        *dest = *srs;
    }

    return 0;
}


is_debug_lvl_1(
    static unsigned int hashCalc(Stack* stk) 
    {
        CHECK_PTR(stk)

        stk->hash = 0;
        stk->hash = MurmurHash2((char*) stk, sizeof(Stack)) + 
        MurmurHash2((char*) stk->data, 2 * sizeof(u_int64_t) + stk->capacity * sizeof(type));

        return stk->hash;
    }
)


static unsigned int MurmurHash2(char* key, unsigned int len) 
{
    // большими буквами хекс
    const unsigned int m = 0x5BD1E995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ len;

    const unsigned char* data = (const unsigned char*) key;
    unsigned int k = 0;

    while (len >= 4) {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };
    
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

is_debug_lvl_0(
    void printError(int Error) 
    {
        if (Error & 1) 
            fprintf(logs, "ERR_CALLING_FUNC_FAILED ");
        if (Error & (1 << 1))
            fprintf(logs, "ERR_STACK_ALREARY_CREATED ");
        if (Error & (1 << 2))
            fprintf(logs, "ERR_EMPTY_ELEM_ISNT_POISONED ");
        if (Error & (1 << 3))
            fprintf(logs, "ERR_SIZE_GREATER_CAPACITY ");
        if (Error & (1 << 4))
            fprintf(logs, "ERR_POP_EMPTY_STACK ");
        if (Error & (1 << 5))
            fprintf(logs, "ERR_RIGHT_CANARY_DAMAGED ");
        if (Error & (1 << 6))
            fprintf(logs, "ERR_LEFT_CANARY_DAMAGED ");
        if (Error & (1 << 7))
            fprintf(logs, "ERR_WRONG_HASH ");
        if (Error & (1 << 8))
            fprintf(logs, "ERR_INVALID_PTR ");
        if (Error & (1 << 9))
            fprintf(logs, "ERR_REALLOC_FAILED ");
        if (Error & (1 << 10))
            fprintf(logs, "ERR_PUSH_FAILED ");
        if (Error & (1 << 11))
            fprintf(logs, "ERR_STACK_ALREADY_CLEANED ");
        if (Error & (1 << 12))
            fprintf(logs, "ERR_LEFT_DATACANARY_DAMAGED ");
        if (Error & (1 << 13))
            fprintf(logs, "ERR_RIGHT_DATACANARY_DAMAGED ");
        if (Error & (1 << 14))
            fprintf(logs, "ERR_RESIZE_FAILED ");
        if (Error & (1 << 15))
            fprintf(logs, "ERR_POP_FAILED ");
        if (Error & (1 << 16))
            fprintf(logs, "ERR_DUMP_FAILED ");
        if (Error & (1 << 17))
            fprintf(logs, "ERR_STACK_BROKEN ");
    }
)

is_debug_lvl_1(
    static void setCanary(Stack* stk, int is_left)
    {
        if (is_left == 1) {
            *((u_int64_t*) LEFT_CANARY(stk)) = CANARY(stk);
        }
        else {
            *((u_int64_t*) RIGHT_CANARY(stk)) = CANARY(stk);
        }
    }
)
