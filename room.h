// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

#ifndef Included_room_h
#define Included_room_h
#include<stddef.h>
#include<stdint.h>

struct share_room
{
    size_t rccapa, rccnt;
    uint64_t num;
    int *receivers;
    int uploader;
};

struct room_htable
{
    struct share_room **buckets;
    size_t bcnt, rcnt;
};

extern struct room_htable fs_all_rooms;
void *process_room(void *arg);
int init_rmtable(void);
int insert_room(uint64_t num, int uploader);
void remove_room(uint64_t num);
void remove_receipient(struct share_room *room, int rec);
int join_room(struct share_room *room, int rec);
struct share_room **get_room(uint64_t num);
int init_room(struct share_room *room, int uploader);
void clear_room(struct share_room *room);
size_t room_cnt(void);

#endif
