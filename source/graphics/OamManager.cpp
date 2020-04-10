//--------------------------------------------------------------------------------
// OamManager.cpp
//--------------------------------------------------------------------------------
// Provides a convenient way to push sprites into the drawing queue, as well
// as sorting them
//--------------------------------------------------------------------------------
#include "OamManager.hpp"

//#define DISABLE_OAM_SORTING
void OamManager::pushAttrs(u16 attr0, u16 attr1, u16 attr2, u16 prio)
{
    ASSERT(objCount < MaxObjs);
    shadowOAM[objCount].attr0 = attr0;
    shadowOAM[objCount].attr1 = attr1;
    shadowOAM[objCount].attr2 = attr2;
    shadowOAM[objCount].fill = (prio << 7) | objCount;
    objCount++;
}

void sortOAM(OBJ_ATTR* shadowOAM, u32 objCount) IWRAM_CODE;

void OamManager::copyToOAM()
{
#ifndef DISABLE_OAM_SORTING
    // Sort the objects
    sortOAM(shadowOAM, objCount);
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