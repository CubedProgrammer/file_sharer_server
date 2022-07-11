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

// Roomless clients list
struct ll_node *volatile rl_client_head, *volatile rl_client_tail;

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
        getchar();
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
                        if(rl_client_tail == NULL)
                        {
                            rl_client_tail = node;
                            pthread_create(&pth, NULL, client_handler, NULL);
                        }
                        else
                        {
                            insert_after(rl_client_tail, node);
                            rl_client_tail = rl_client_tail->ne;
                        }
                    }
                }
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
    char msgnam[13];
    fs_msg_t msgt;
    struct ll_node *node;
    while(rl_client_tail != NULL)
    {
        big = 0;
        FD_ZERO(fdsp);
        for(node = rl_client_head; node != NULL; node = node->ne)
        {
            big = big < node->val ? node->val : big;
            FD_SET(node->val, fdsp);
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
                switch(msgt)
                {
                    case UPLOADER:
                        break;
                    case RECEIPIENT:
                        break;
                    default:
                        get_msg_name(msgt, msgnam);
                        logfmt("Invalid message %s, should be UPLOADER or RECEIPIENT.\n", msgnam);
                        log_endmsg();
                }
            }
        }
    }
    return NULL;
}
void sigpipe(int x)
{
    logstr("SIGPIPE happened");
    log_endmsg();
}
