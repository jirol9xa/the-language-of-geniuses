#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Suff_tree.h"
#include "Tree.h"
#include "Recursive_descent.h"
#include "Lexer.h"
#include "LogsLib.h"

int lexAnal(Tokens_t *tokens)
{
    assert(tokens);

    Suff_Tree *suff_tree = suffTreeCtor();

    int insd_rnd_brack = 0;
    int insd_fig_brack = 0;

    PRINT_LINE;

    for (int i = 0; i < tokens->size; i++)
    {
        Node *token  = tokens->array[i];
        bool  is_num = (token->node_type.bytes.is_number);

        insd_rnd_brack += (!is_num && token->value.str[0] == '(');
        insd_rnd_brack -= (!is_num && token->value.str[0] == ')');

        insd_fig_brack += (!is_num && token->value.str[0] == '{');
        insd_fig_brack -= (!is_num && token->value.str[0] == '}');

        if (token->node_type.bytes.is_variable)
        {
            PRINT_LINE;
            int status = isKeyword(token->value.str, suff_tree);
            PRINT_LINE;
            token->node_type.number = status + !status * (1 << 9);

            if (i < (tokens->size - 1) && !status && tokens->array[i + 1]->value.str[0] == '(')
            {
                token->node_type.number = 0;
                token->node_type.bytes.is_func = 1;
            }
        }
    }
    PRINT_LINE;
    if (insd_fig_brack || insd_rnd_brack)
    {
        SyntaxError();
    }
    
    PRINT_LINE;

    for (int i = 0; i < tokens->size; i++)
    {
        if (tokens->array[i]->node_type.bytes.is_number)
        {
            printf("val = %lg, status = %d\n", tokens->array[i]->value.number, tokens->array[i]->node_type.number);
            continue;
        }

        printf("str = %s, status = %d\n", tokens->array[i]->value.str, tokens->array[i]->node_type.number);
    }

    PRINT_LINE;

    suffTreeDtor(suff_tree);
    return 0;
}