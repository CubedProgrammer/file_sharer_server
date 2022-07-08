#ifndef Included_queue_h
#define Included_queue_h

typedef int ll_val_t;

struct ll_node
{
    int val;
    struct ll_node *ne, *pr;
};

void remove_node(struct ll_node *n);
void insert_after(struct ll_node *x, struct ll_node *y);
struct ll_node *make_node(ll_val_t val, struct ll_node *next, struct ll_node *prev);

#endif
