//--------------------------------------------------------------------------------
// OamManager.cpp
//--------------------------------------------------------------------------------
// Provides a convenient way to push sprites into the drawing queue, as well
// as sorting them
//--------------------------------------------------------------------------------
#include "OamManager.hpp"
#include <algorithm>

//#define DISABLE_OAM_SORTING

void OamManager::copyToOAM()
{
#ifndef DISABLE_OAM_SORTING
    // Sort the objects
    std::sort(shadowOAM, shadowOAM + objCount,
        [](const OBJ_ATTR& o1, const OBJ_ATTR& o2) { return o1.fill < o2.fill; });
#endif

    // Copy the shadow OAM
    oam_copy(oam_mem, shadowOAM, MaxObjs);

    // "Reset" the OAM by hiding all objects
    while (objCount)
    {
        objCount--;
        shadowOAM[objCount].attr0 = ATTR0_HIDE;
    }
}