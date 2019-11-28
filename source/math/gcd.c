//--------------------------------------------------------------------------------
// gcd.c
//--------------------------------------------------------------------------------
// Provides a binary GCD algorithm
//--------------------------------------------------------------------------------
#include "intmath.h"

u32 gcd(u32 x, u32 y)
{
    u32 shift = 0;

    // Loop untill one of them is zero
    for (;;)
    {
        if (y == 0) return x << shift;
        if (x == 0) return y << shift;

        // The five cases
        if ((x&1) == 0 && (y&1) == 0) x >>= 1, y >>= 1, shift++;
        else if ((x&1) == 0) x >>= 1;
        else if ((y&1) == 0) y >>= 1;
        else if (x >= y) x = (x-y) >> 1;
        else y = (y-x) >> 1;
    }
}