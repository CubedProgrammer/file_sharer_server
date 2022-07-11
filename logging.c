#include<stdarg.h>
#include<stdio.h>
#include<time.h>
#include"logging.h"

FILE *log_fh;
char log_msg;
time_t log_start;

void logstr(const char *str)
{
    logfmt("%s\n", str);
}

void logfmt(const char *fmt, ...)
{
    if(!log_msg)
    {
        log_msg = 1;
        time_t curr = time(NULL);
        int daystart = log_start / 86400, daycurr = curr / 86400;
        int secs = curr % 86400;
        int mins = curr / 60 % 60, hrs = curr / 3600;
        curr %= 60;
        fprintf(log_fh, "Day %d [%02d:%02d:%02d] ", daycurr - daystart, hrs, mins, secs);
    }
    va_list ls;
    va_start(ls, fmt);
    vfprintf(log_fh, fmt, ls);
    va_end(ls);
}

void log_endmsg(void)
{
    fflush(log_fh);
    log_msg = 0;
}

int init_logger(const char *logfn)
{
    int succ = 0;
    time(&log_start);
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
