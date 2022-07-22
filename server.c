// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

#include<arpa/inet.h>
#include<errno.h>
#include<pthread.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<time.h>
#include<unistd.h>
#include"logging.h"
#include"msg.h"
#include"queue.h"
#include"rand.h"
#include"room.h"
#ifndef PORT
#define PORT 2203
#endif

void *client_accepter(void *arg);
void *client_handler(void *arg);
void sigpipe(int x);
int main(int argl, char *argv[])
{
    printf("%s has begun.\n", argv[0]);
    signal(SIGPIPE, &sigpipe);
    init_rand();
    int succ = init_rmtable();
    if(succ == 0)
    {
        succ = init_logger("logs.txt");
        if(succ == 0)
        {
            pthread_t pth;
            pthread_create(&pth, NULL, client_accepter, NULL);
            getchar();
            free_logger();
        }
    }
   return succ * -1;
}
void *client_accepter(void *arg)
{
    struct sockaddr_in addrin, *addrinp = &addrin;
    socklen_t len = sizeof addrin;
    addrin.sin_family = AF_INET;
    addrin.sin_port = htons(PORT);
    addrin.sin_addr.s_addr = INADDR_ANY;
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if(server > 0)
    {
        int succ = bind(server, (struct sockaddr *)addrinp, len);
        if(succ == 0)
        {
            succ = listen(server, 3);
            if(succ == 0)
            {
                struct ll_node *node;
                pthread_t pth;
                int cfd = 1;
                for(; cfd > 0;)
                {
                    cfd = accept(server, (struct sockaddr *)addrinp, &len);
                    if(cfd < 0)
                        printf("Accepting client failed, %d is errno.", errno);
                    else
                    {
                        node = make_node(cfd, NULL, NULL);
                        if(node == NULL)
                        {
                            logstr("Memory allocation failed for new client.");
                            close(cfd);
                        }
                        else if(rl_client_tail == NULL)
                        {
                            logstr("First client to connect in a while.");
                            rl_client_head = rl_client_tail = node;
                            pthread_create(&pth, NULL, client_handler, NULL);
                        }
                        else
                        {
                            logfmt("Client with socket %d joined the queue.\n", cfd);
                            insert_after(rl_client_tail, node);
                            rl_client_tail = rl_client_tail->ne;
                        }
                        log_endmsg();
                    }
                }
                logstr("Stopped accepting clients.");
                log_endmsg();
            }
            else
                puts("Socket listening failed");
        }
        else
            puts("Socket bind failed");
    }
    else
        puts("Socket could not be created.");
    return NULL;
}
void *client_handler(void *arg)
{
    struct timeval tv, *tvp = &tv;
    fd_set fds, *fdsp = &fds;
    int ready, big, sock;
    int succ;
    uint64_t rnum;
    uint32_t rnumpart;
    pthread_t pth;
    struct share_room *roomp, **roompp;
    char msgnam[13];
    fs_msg_t msgt;
    struct ll_node *node;
    while(rl_client_tail != NULL)
    {
        big = 0;
        FD_ZERO(fdsp);
        while(NULL != rl_client_head && rl_client_head->val == -1)
        {
            node = rl_client_head->ne;
            remove_node(rl_client_head);
            rl_client_head = node;
        }
        if(rl_client_head == NULL)
            rl_client_tail = NULL;
        for(node = rl_client_head; node != NULL; node = node->ne)
        {
            if(node->val == -1)
            {
                if(node == rl_client_tail)
                    rl_client_tail = node->pr;
                node = node->pr;
                remove_node(node->ne);
            }
            else
            {
                big = big < node->val ? node->val : big;
                FD_SET(node->val, fdsp);
            }
        }
        tv.tv_usec = 0;
        tv.tv_sec = 1;
        ready = select(big + 1, fdsp, NULL, NULL, tvp);
        for(node = rl_client_head; node != NULL; node = node->ne)
        {
            if(FD_ISSET(node->val, fdsp))
            {
                sock = node->val;
                GETOBJ(sock, msgt);
                get_msg_name(msgt, msgnam);
                logfmt("Socket %d sent message %s.\n", sock, msgnam);
                log_endmsg();
                switch(msgt)
                {
                    case UPLOADER:
                        rnum = next_long();
                        succ = insert_room(rnum, sock);
                        if(succ == 0)
                        {
                            msgt = ROOMNUM;
                            succ = PUTOBJ(sock, msgt);
                            if(succ == -1)
                            {
                                remove_room(rnum);
                                close(sock);
                            }
                            else
                            {
                                rnumpart = rnum >> 32;
                                rnumpart = htonl(rnumpart);
                                PUTOBJ(sock, rnumpart);
                                rnumpart = rnum;
                                rnumpart = htonl(rnumpart);
                                succ = PUTOBJ(sock, rnumpart);
                                if(succ == -1)
                                {
                                    remove_room(rnum);
                                    close(sock);
                                }
                                else
                                {
                                    logfmt("Successfully created room %lu, there are now %zu rooms.\n", room_cnt());
                                    log_endmsg();
                                    pthread_create(&pth, NULL, process_room, *get_room(rnum));
                                }
                            }
                        }
                        else
                        {
                            logfmt("Inserting room failed, there are %zu rooms.\n", room_cnt());
                            log_endmsg();
                            msgt = QUIT;
                            PUTOBJ(sock, msgt);
                            close(sock);
                        }
                        node->val = -1;
                        break;
                    case RECEIPIENT:
                        GETOBJ(sock, msgt);
                        if(msgt == QUIT)
                        {
                            close(sock);
                            logfmt("Receipient with socket %d quit.\n", sock);
                            log_endmsg();
                        }
                        else if(msgt == ROOMNUM)
                        {
                            GETOBJ(sock, rnumpart);
                            rnumpart = ntohl(rnumpart);
                            rnum = rnumpart;
                            rnum <<= 32;
                            GETOBJ(sock, rnumpart);
                            rnumpart = ntohl(rnumpart);
                            rnum |= rnumpart;
                            logfmt("A client attempted to join room %lx.\n", rnum);
                            roompp = get_room(rnum);
                            if(roompp == NULL)
                            {
                                logstr("And failed.");
                                msgt = JOINFAIL;
                            }
                            else
                            {
                                roomp = *roompp;
                                succ = join_room(roomp, sock);
                                if(succ == 0)
                                {
                                    logfmt("Room %lu has a new receipient, for a total of %zu.\n", roomp->num, roomp->rccnt);
                                    msgt = JOINSUCC;
                                }
                                else
                                    msgt = JOINFAIL;
                            }
                            succ = PUTOBJ(sock, msgt);
                            if(succ == -1)
                            {
                                remove_receipient(roomp, sock);
                                close(sock);
                            }
                            log_endmsg();
                        }
                        else
                        {
                            get_msg_name(msgt, msgnam);
                            logfmt("Invalid message %s, expected ROOMNUM.\n", msgnam);
                            log_endmsg();
                            close(sock);
                        }
                        node->val = -1;
                        break;
                    case QUIT:
                        close(sock);
                        logfmt("Receipient on socket %d quit immediately.\n", sock);
                        log_endmsg();
                        break;
                    default:
                        close(sock);
                        get_msg_name(msgt, msgnam);
                        logfmt("Invalid message %s, should be UPLOADER or RECEIPIENT.\n", msgnam);
                        log_endmsg();
                }
            }
        }
    }
    logstr("No more clients in the queue.");
    log_endmsg();
    return NULL;
}
void sigpipe(int x)
{
    logstr("SIGPIPE happened");
    log_endmsg();
}
