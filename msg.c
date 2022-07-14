// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

#include<string.h>
#include"msg.h"

void get_msg_name(fs_msg_t msg, char *buf)
{
    const char *name;
    switch(msg)
    {
        case UPLOADER:
            name = "UPLOADER";
            break;
        case RECEIPIENT:
            name = "RECEIPIENT";
            break;
        case ROOMNUM:
            name = "ROOMNUM";
            break;
        case JOINSUCC:
            name = "JOINSUCC";
            break;
        case JOINFAIL:
            name = "JOINFAIL";
            break;
        case QUIT:
            name = "QUIT";
            break;
        case SENDFILE:
            name = "SENDFILE";
            break;
        case CLOSEROOM:
            name = "CLOSEROOM";
            break;
        default:
            name = "UNKNOWN";
    }
    strcpy(buf, name);
}
