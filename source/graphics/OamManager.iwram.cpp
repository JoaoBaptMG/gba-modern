//--------------------------------------------------------------------------------
// OamManager.cpp
//--------------------------------------------------------------------------------
// Provides a convenient way to push sprites into the drawing queue, as well
// as sorting them
//--------------------------------------------------------------------------------
#include "OamManager.hpp"
#include <algorithm>

void sortOAM(OBJ_ATTR* shadowOAM, u32 objCount)
{
    std::sort(shadowOAM, shadowOAM + objCount,
        [](const OBJ_ATTR& o1, const OBJ_ATTR& o2) { return o1.fill < o2.fill; });
}