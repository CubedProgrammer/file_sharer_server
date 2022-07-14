// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

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
