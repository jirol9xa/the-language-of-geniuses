#ifndef LIST_H
    #define LIST_H

    typedef Name* type_t;

    struct elem_t
    {
        type_t value;
        int    prev;
        int    next;
    };

    struct List
    {
        elem_t   *array;
        int       tail;
        int       head;
        int       size;
        int       capacity;
        int       status;
        int       free_head;
    };

    /*!
        \brief  Функция создания списка
        \param  [List *]list Указатель на список
        \param  [int]capacity Число эелментов
                в списке
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int listCtor(List *list, int capacity);
    /*!
        \brief  Функция уничтожения списка
        \param  [List *]list Указатель на список
        \return 0 в случае успехи, 1 в противном
                случае
    */
    int listDtor(List *list);
    /*!
        \brief  Функция поиска свободной 
                ячейки в списке
        \param  [List *]list Указатель на списко
        \return 0 в случае успеха, 2 в противном 
                случае
    */
    int searchFree(List *list);
    /*!
        \brief  Функция добавление элемента
                в конец списка
        \param  [List *]list Указатель на список
        \param  [type_t]value Элемент, который
                нужно добавить
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int listPushBack(List *list, type_t value);
    /*!
        \brief  Функция добавления элемента в 
                произвольное место списка
        \param  [List *]list указатель на список
        \param  [type_t]value Элемент, который
                нужно добавить
        \param  [int]place Номер элемента, после
                которого нужно добавить наш
        \return 0 в случае успеха, 1 в противном случае
    */
    int listInsertAfter(List* list, type_t value, int place);
    /*!
        \brief  Функция добавления элемента в 
                произвольное место списка
        \param  [List *]list указатель на список
        \param  [type_t]value Элемент, который
                нужно добавить
        \param  [int]place Номер элемента, до
                которого нужно добавить наш
        \return 0 в случае успеха, 1 в противном случае
    */
    int listInsertBefore(List* list, type_t value, int place);
    /*!
        \brief  Функция извлечения элемента с 
                конца списка
        \param  [List *]list Указатель на список
        \param  [tyoe_t *]dest Указатель на переменную,
                куда надо извлечь элемент
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int listPopBack(List *list, type_t *dest);
     /*!
        \brief  Функция извлечения элемента с 
                начала списка
        \param  [List *]list Указатель на список
        \param  [tyoe_t *]dest Указатель на переменную,
                куда надо извлечь элемент
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int listPopFront(List *list, type_t *dest);

    /*!
        \brief  Функция проверки списка на 
                корректность
        \param  [List *]list Указатель на список
    */
    int verifyList(List *list);
    /*!
        \brief  Функция вывода информации о
                списке
        \param  [List *]list Указатель на список
        \return 0 в случае успеха, 1 в противном
                случае
    */
    //void listTextDump(List *list);
    //void listGraphDump(List *list);
    /*!
        \brief  Функция линеризации списка
        \param  [List *]list Указатель на список
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int    listLinearization(List *list);
    int    listResize       (List *list, int   is_upper);
    int    listRemove       (List *list, int   index, type_t *dest);
    int    findPlace        (List *list, int   index);
    type_t findElem         (List *list, char *elem);
    int    unitTest();


    enum STATUS_AND_ERRS
    {
        FULL_LIST          = 1 << 0,
        EMPTY_LIST         = 1 << 1,
        LINEARIZATED       = 1 << 2,
        EMPTY_ELEM_ERROR   = 1 << 10,
        DISJOINTED_LIST    = 1 << 11,
        NOT_RESIZED        = 1 << 12,
    };


    /*#define LIST_DUMP(list)                                                             \
        verifyList(list);                                                                 \
        openLogs("LOGS/logs");                                                            \
        writeLogs("In func -----> %s\n", __func__);                                       \
        closeLogs();                                                                      \
        listTextDump(list);                                                               \
        //listGraphDump(list);         */                                                     

    #define ASSERT_OK(list)                                                               \
        if (list->status >> 10)                                                           \
        {                                                                                 \
            return list->status;                                                          \
        }      


    const int POISON = -1;

#endif