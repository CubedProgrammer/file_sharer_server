#include<stdlib.h>
#include"queue.h"

// Roomless clients list
struct ll_node *volatile rl_client_head, *volatile rl_client_tail;

void remove_node(struct ll_node *n)
{
    if(n->pr)
        n->pr->ne = n->ne;
    if(n->ne)
        n->ne->pr = n->pr;
    free(n);
}

void insert_after(struct ll_node *x, struct ll_node *y)
{
    struct ll_node *n = x->ne;
    if(n != NULL)
    {
        y->ne = n;
        n->pr = y;
    }
    y->pr = x;
    x->ne = y;
}

struct ll_node *make_node(ll_val_t val, struct ll_node *next, struct ll_node *prev)
{
    struct ll_node *n = malloc(sizeof(*n));
    if(n != NULL)
    {
        n->val = val;
        n->ne = next;
        n->pr = prev;
    }
    return n;
}
