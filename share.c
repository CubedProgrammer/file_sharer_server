// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include"logging.h"
#include"msg.h"

void share_file(int uploader, int *rc, size_t rccnt, size_t bufsz)
{
    char unsigned fszdat[8];
    size_t fsz = 0, br;
    br = read(uploader, fszdat, sizeof fszdat);
    if(br < 8)
    {
        logfmt("Uploader %d send %zu bytes for file size instead of eight.\n", uploader, br);
        log_endmsg();
    }
    else
    {
        for(size_t i = 0; i < sizeof(fszdat); ++i)
            fsz = (fsz << 8) + fszdat[i];
        bufsz = bufsz < fsz ? bufsz : fsz;
        char msgt = SENDFILE;
        size_t totbr;
        ssize_t bc;
        for(size_t i = 0; i < rccnt; ++i)
        {
            PUTOBJ(rc[i], msgt);
            bc = write(rc[i], fszdat, sizeof fszdat);
            if(bc < 0)
            {
                logfmt("Writing file size to socket %d failed, errno %d.\n", rc[i], errno);
                log_endmsg();
                close(rc[i]);
                rc[i] = -1;
            }
        }
        totbr = 0;
        void *buf = malloc(bufsz);
        while(totbr < fsz)
        {
            br = read(uploader, buf, bufsz);
            for(size_t i = 0; i < rccnt; ++i)
            {
                if(rc[i] == -1)
                    continue;
                bc = write(rc[i], buf, br);
                if(bc < 0)
                {
                    logfmt("Writing file data to socket %d failed, errno %d.\n", rc[i], errno);
                    log_endmsg();
                    close(rc[i]);
                    rc[i] = -1;
                }
            }
            totbr += br;
        }
        free(buf);
    }
}
