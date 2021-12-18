#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Lexer.h"
#include "Suff_tree.h"
#include "Tree.h"
#include "Recursive_descent.h"

int lexAnal(Tokens_t *tokens)
{
    assert(tokens);

    Suff_Tree *suff_tree = suffTreeCtor();

    int insd_rnd_brack = 0;
    int insd_fig_brack = 0;

    for (int i = 0; i < tokens->size; i++)
    {
        Node *token = tokens->array[i];

        insd_rnd_brack += (token->value.str[0] == '(');
        insd_rnd_brack -= (token->value.str[0] == ')');

        insd_fig_brack += (token->value.str[0] == '{');
        insd_fig_brack -= (token->value.str[0] == '}');

        if (token->node_type.bytes.is_variable)
        {
            int status = isKeyword(token->value.str, suff_tree);

            token->node_type.number = status + !status;

            if (!status && tokens->array[i + 1]->value.str[0] == '(')
            {
                token->node_type.number = 0;
                token->node_type.bytes.is_func = 1;
            }
        }
    }

    if (insd_fig_brack || insd_rnd_brack)
    {
        SyntaxError();
    }

    for (int i = 0; i < tokens->size; i++)
    {
        printf("str = %s, status = %d\n", tokens->array[i]->value.str, tokens->array[i]->node_type.number);
    }

    suffTreeDtor(suff_tree);
    return 0;
}