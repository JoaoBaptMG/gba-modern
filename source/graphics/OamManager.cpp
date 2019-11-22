//--------------------------------------------------------------------------------
// OamManager.cpp
//--------------------------------------------------------------------------------
// Provides a convenient way to push sprites into the drawing queue, as well
// as sorting them
//--------------------------------------------------------------------------------

#include "OamManager.hpp"

void OamManager::copyToOAM()
{
    // Copy the shadow OAM
    oam_copy(oam_mem, shadowOAM, MaxObjs);

    // "Reset" the OAM by hiding all objects
    while (objCount)
    {
        objCount--;
        shadowOAM[objCount].attr0 = ATTR0_HIDE;
    }
}