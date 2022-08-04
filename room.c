// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

#include<errno.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<sys/select.h>
#include<time.h>
#include<unistd.h>
#include"logging.h"
#include"msg.h"
#include"queue.h"
#include"room.h"
#include"share.h"

struct room_htable fs_all_rooms;
void *client_handler(void *arg);

void *process_room(void *arg)
{
    struct share_room *roomp = arg;
    logfmt("Room %lx has begun.\n", roomp->num);
    log_endmsg();
    struct ll_node *node;
    struct timeval tv, *tvp = &tv;
    fd_set fds, *fdsp = &fds;
    size_t shift, bufsz = 16384;
    char bufszstr[11];
    char *bufszstrog; 
    fs_msg_t msgt;
    char msgnam[13];
    int big, fd;
    int ready, succ;
    pthread_t pth;
    char fini = 0;
    while(!fini)
    {
        big = roomp->uploader;
        FD_ZERO(fdsp);
        FD_SET(big, fdsp);
        for(size_t i = 0; i < roomp->rccnt; ++i)
        {
            fd = roomp->receivers[i];
            FD_SET(fd, fdsp);
            big = fd > big ? fd : big;
        }
        tv.tv_usec = 0;
        tv.tv_usec = 5;
        ready = select(big + 1, fdsp, NULL, NULL, tvp);
        if(ready < 0)
        {
            logfmt("Select failed for room %lx, errno is %d.\n", roomp->num, errno);
            log_endmsg();
            fini = 1;
        }
        else if(ready)
        {
            shift = 0;
            for(size_t i = 0; i < roomp->rccnt; ++i)
            {
                if(shift)
                    roomp->receivers[i] = roomp->receivers[i + shift];
                fd = roomp->receivers[i];
                if(FD_ISSET(fd, fdsp))
                {
                    GETOBJ(fd, msgt);
                    if(msgt != QUIT)
                    {
                        get_msg_name(msgt, msgnam);
                        logfmt("Invalid message %s, only QUIT is allowed for receipients.\n", msgnam);
                        log_endmsg();
                        msgt = QUIT;
                        PUTOBJ(fd, msgt);
                    }
                    --roomp->rccnt;
                    --i;
                    ++shift;
                    close(fd);
                }
            }
            fd = roomp->uploader;
            if(FD_ISSET(fd, fdsp))
            {
                GETOBJ(fd, msgt);
                switch(msgt)
                {
                    case SENDFILE:
                        bufszstrog = getenv("SHAREBUFSZ");
                        if(bufszstrog != NULL)
                        {
                            strcpy(bufszstr, bufszstrog);
                            bufsz = strtoul(bufszstr, NULL, 10);
                        }
                        share_file(fd, roomp->receivers, roomp->rccnt, bufsz);
                        shift = 0;
                        for(size_t i = 0; i < roomp->rccnt; ++i)
                        {
                            if(roomp->receivers[i] == -1)
                                ++shift;
                            if(shift)
                                roomp->receivers[i] = roomp->receivers[i + shift];
                        }
                        roomp->rccnt -= shift;
                        break;
                    case QUIT:
                        close(fd); 
                    case CLOSEROOM:
                        for(size_t i = 0; i < roomp->rccnt; ++i)
                        {
                            fd = roomp->receivers[i];
                            succ = PUTOBJ(fd, msgt);
                            if(succ == -1)
                            {
                                logfmt("Writing to socket %d of room %lx failed, errno %d.\n", fd, roomp->num, errno);
                                log_endmsg();
                            }
                            close(fd);
                        }
                        roomp->rccnt = 0;
                        fd = roomp->uploader;
                        if(msgt == CLOSEROOM)
                        {
                            logfmt("Room %lx ordered to be closed.\n", roomp->num);
                            log_endmsg();
                            node = make_node(fd, NULL, rl_client_tail);
                            if(node == NULL)
                            {
                                logstr("Could not create node to add client back to waiting list.");
                                log_endmsg();
                                close(fd);
                            }
                            else if(rl_client_tail == NULL)
                            {
                                rl_client_head = rl_client_tail = node;
                                pthread_create(&pth, NULL, client_handler, NULL);
                            }
                            else
                            {
                                insert_after(rl_client_tail, node);
                                rl_client_tail = node;
                            }
                        }
                        fini = 1;
                        break;
                    default:
                        get_msg_name(msgt, msgnam);
                        logfmt("Invalid message %s, only QUIT, SENDFILE, and CLOSEROOM are allowed.\n", msgnam);
                        logfmt("Disconnecting all clients of room %lx.\n", roomp->num);
                        log_endmsg();
                        msgt = QUIT;
                        for(size_t i = 0; i < roomp->rccnt; ++i)
                        {
                            fd = roomp->receivers[i];
                            PUTOBJ(fd, msgt);
                            close(fd);
                        }
                        fd = roomp->uploader;
                        PUTOBJ(fd, msgt);
                        close(fd);
                        fini = 1;
                }
            }
        }
    }
    logfmt("Room %lx has ended.\n", roomp->num);
    log_endmsg();
    remove_room(roomp->num);
    return NULL;
}

int init_rmtable(void)
{
    int succ = 0;
    fs_all_rooms.bcnt = 40;
    fs_all_rooms.rcnt = 0;
    fs_all_rooms.buckets = malloc(fs_all_rooms.bcnt * sizeof(*fs_all_rooms.buckets));
    if(fs_all_rooms.buckets == NULL)
        succ = -1;
    else
        memset(fs_all_rooms.buckets, 0, fs_all_rooms.bcnt * sizeof(*fs_all_rooms.buckets));
    return succ;
}

int insert_room(uint64_t num, int uploader)
{
    int succ = 0;
    if(fs_all_rooms.rcnt * 5 > fs_all_rooms.bcnt * 3)
    {
        size_t ncnt = fs_all_rooms.bcnt * 3 / 2;
        struct share_room **new = malloc(sizeof(*new) * ncnt);
        if(new != NULL)
        {
            size_t ind, hsh;
            uint64_t rnum;
            memset(new, 0, sizeof(*new) * ncnt);
            for(size_t i = 0; i < fs_all_rooms.bcnt; ++i)
            {
                if(fs_all_rooms.buckets[i] != NULL)
                {
                    hsh = fs_all_rooms.buckets[i]->num;
                    ind = hsh %= ncnt;
                    for(; new[ind] != NULL; ++ind)
                    {
                        if(ind == ncnt - 1)
                            ind = -1;
                    }
                    new[ind] = fs_all_rooms.buckets[i];
                }
            }
            free(fs_all_rooms.buckets);
            fs_all_rooms.buckets = new;
            fs_all_rooms.bcnt = ncnt;
        }
        else
            succ = -1;
    }
    if(succ == 0)
    {
        struct share_room *rm = malloc(sizeof(*rm));
        if(rm != NULL)
        {
            init_room(rm, uploader);
            rm->num = num;
            num %= fs_all_rooms.bcnt;
            for(; fs_all_rooms.buckets[num] != NULL; ++num)
            {
                if(num == fs_all_rooms.bcnt - 1)
                    num = -1;
            }
            fs_all_rooms.buckets[num] = rm;
            ++fs_all_rooms.rcnt;
        }
        else
            succ = -1;
    }
    return succ;
}

void remove_room(uint64_t num)
{
    struct share_room **rmp = get_room(num);
    if(rmp != NULL)
    {
        clear_room(*rmp);
        --fs_all_rooms.rcnt;
    }
    free(*rmp);
}

void remove_receipient(struct share_room *room, int rec)
{
    char found = 0;
    for(size_t i = 0; i < room->rccnt; ++i)
    {
        if(room->receivers[i] == rec)
        {
            found = 1;
            --room->rccnt;
        }
        if(found)
            room->receivers[i] = room->receivers[i + 1];
    }
}

int join_room(struct share_room *room, int rec)
{
    int succ = 0;
    if(room->rccnt == room->rccapa)
    {
        room->rccapa += room->rccapa >> 1;
        int *new = malloc(room->rccapa * sizeof(*new));
        if(new == NULL)
            succ = -1;
        else
        {
            memcpy(new, room->receivers, room->rccnt * sizeof(*new));
            free(room->receivers);
            room->receivers = new;
        }
    }
    if(succ == 0)
    {
        room->receivers[room->rccnt] = rec;
        ++room->rccnt;
    }
    return succ;
}

struct share_room **get_room(uint64_t num)
{
    uint64_t hsh = num % fs_all_rooms.bcnt;
    struct share_room **rmp = NULL;
    if(fs_all_rooms.buckets[hsh] == NULL)
        rmp = NULL;
    else if(fs_all_rooms.buckets[hsh]->num == num)
        rmp = fs_all_rooms.buckets + hsh;
    else
    {
        for(size_t ind = hsh + 1; ind != hsh; ++ind)
        {
            if(fs_all_rooms.buckets[ind] == NULL)
                ind = hsh - 1;
            else if(fs_all_rooms.buckets[ind]->num == num)
            {
                rmp = fs_all_rooms.buckets + ind;
                ind = hsh - 1;
            }
            if(1 + ind == fs_all_rooms.bcnt)
                ind = -1;
        }
    }
    return rmp;
}

int init_room(struct share_room *room, int uploader)
{
    room->rccnt = 0;
    room->rccapa = 24;
    room->receivers = malloc(room->rccapa * sizeof(*room->receivers));
    room->uploader = uploader;
    return room->receivers == NULL;
}

void clear_room(struct share_room *room)
{
    free(room->receivers);
    room->num = 0;
}

size_t room_cnt(void)
{
    return fs_all_rooms.rcnt;
}
