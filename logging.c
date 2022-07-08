#include<stdio.h>

FILE *log_fh;
int init_logger(const char *logfn)
{
    int succ = 0;
    log_fh = fopen(logfn, "a");
    if(log_fh == NULL)
        succ = -1;
    return succ;
}

void free_logger(void)
{
    if(log_fh)
        fclose(log_fh);
}
