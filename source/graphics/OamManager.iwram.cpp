//--------------------------------------------------------------------------------
// OamManager.cpp
//--------------------------------------------------------------------------------
// Provides a convenient way to push sprites into the drawing queue, as well
// as sorting them
//--------------------------------------------------------------------------------
#include "OamManager.hpp"
#include <algorithm>

#define STACKALLOC(T, n) (T*)__builtin_alloca(sizeof(T) * (n))

void sortOAM(OBJ_ATTR* dstOAM, const OBJ_ATTR* srcOAM, u32 objCount)
{
    // Construct a basic array of prios and ids
    auto prioids = STACKALLOC(u32, objCount);
    for (u32 i = 0; i < objCount; i++)
        prioids[i] = (srcOAM[i].fill << 16) | i;

    // Sort the ids - this will ensure stable sort
    std::sort(prioids, prioids+objCount);

    // Push the ids back
    for (u32 i = 0; i < objCount; i++)
        dstOAM[i] = srcOAM[prioids[i] & 0xFF];
}