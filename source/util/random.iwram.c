//--------------------------------------------------------------------------------
// random.c
//--------------------------------------------------------------------------------
// Provides a simple random number generator
//--------------------------------------------------------------------------------
#include "random.h"

unsigned long long randomState;

u32 IWRAM_CODE random()
{
    randomState ^= randomState >> 11;
    randomState ^= randomState << 31;
    randomState ^= randomState >> 18;
    u32 result = (randomState * 0xd989bcacc137dcd5ULL) >> 32;
    return result;
}