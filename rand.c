#include<time.h>
#include<unistd.h>
#include"rand.h"

uint64_t rngnum;
void init_rand(void)
{
    rngnum = time(NULL);
    rngnum *= rngnum * getpid();
    rngnum ^= SLOPE;
}

uint64_t next_long(void)
{
    uint64_t x = next_int();
    x <<= 32;
    return x | next_int();
}

uint32_t next_int(void)
{
    rngnum *= SLOPE;
    rngnum += YINT;
    rngnum %= MODULUS;
    return rngnum;
}
