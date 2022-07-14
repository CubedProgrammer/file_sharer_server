// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

#ifndef Included_queue_h
#define Included_queue_h

typedef int ll_val_t;

struct ll_node
{
    int val;
    struct ll_node *ne, *pr;
};

// Roomless clients list
extern struct ll_node *volatile rl_client_head, *volatile rl_client_tail;

void remove_node(struct ll_node *n);
void insert_after(struct ll_node *x, struct ll_node *y);
struct ll_node *make_node(ll_val_t val, struct ll_node *next, struct ll_node *prev);

#endif
