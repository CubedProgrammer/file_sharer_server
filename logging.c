// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

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
