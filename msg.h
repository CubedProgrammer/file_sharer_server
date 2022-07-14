// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

#ifndef Included_msg_h
#define Included_msg_h
#define UPLOADER 13
#define RECEIPIENT 17
#define ROOMNUM 19
#define JOINSUCC 23
#define JOINFAIL 29
#define QUIT 31
#define SENDFILE 37
#define CLOSEROOM 41
#define GETOBJ(filedes, var)read(filedes, &var, sizeof var)
#define PUTOBJ(filedes, var)write(filedes, &var, sizeof var)

typedef char unsigned fs_msg_t;
void get_msg_name(fs_msg_t msg, char *buf);

#endif
