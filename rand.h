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
