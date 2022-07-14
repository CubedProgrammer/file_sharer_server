// This file is part of file_sharer_server.
// Copyright (C) 2022, github.com/CubedProgrammer, owner of said account.

// file_sharer_server is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// file_sharer_server is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with file_sharer_server. If not, see <https://www.gnu.org/licenses/>. 

#ifndef Included_rand_h
#define Included_rand_h
#include<stdint.h>
#define SLOPE 25214903917
#define YINT 11
#define MODULUS (1ul << 48)

void init_rand(void);
uint64_t next_long(void);
uint32_t next_int(void);

#endif
