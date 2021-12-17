#ifndef SUFF_H
    #define SUFF_H

    struct Suff_node;
    // hello vovqa, i love you so fuckin much

    /*!
        \brief Структура ребра для суффиксального дерева
        \param [int] start первая позиция символа в строке
               зарезервированный команд
        \param [int] length длина подстроки в строке
               зарезервированных символов
        ]param [Node_suff *] Указатель на следющей узел
    */
    struct Edge
    {
        int        start;
        int        length;
        int        status;
        Suff_node *next;
    };
    // hello vovqa, i love you so fuckin much

    /*!
        \brief Структура узла для суффиксального дерева
        \param [Edges *] Ребра на все буквы
    */
    struct Suff_node
    {
        Edge *edges[26];
    };
    // hello vovqa, i love you so fuckin much

    /*!
        \brief Суффиксальное дерево
        \param [Node_suff *] root Указатель на корень дерева
    */
    struct Suff_Tree
    {
        Suff_node *root;
    };
    // hello vovqa, i love you so fuckin much

    
    Suff_node *suffNodeCtor();
    Edge      *edgeCtor();
    Suff_Tree *suffTreeCtor();
    int        suffTreeDtor(Suff_Tree *tree);
    int        suffDump(Suff_Tree *tree);
    void       printSuffTree(Suff_node *node);
    int        isKeyword(char *cmd, Suff_Tree *tree);

#endif