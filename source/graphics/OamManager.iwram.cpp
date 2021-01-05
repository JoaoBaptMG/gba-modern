//--------------------------------------------------------------------------------
// OamManager.cpp
//--------------------------------------------------------------------------------
// Provides a convenient way to push sprites into the drawing queue, as well
// as sorting them
//--------------------------------------------------------------------------------
#include "OamManager.hpp"
#include <algorithm>
#include <tonc.h>
#include "text/mGBADebugging.hpp"

void OamManager::sortOAM()
{
    // Here, we'll do two bubble sort phase
    bool notSorted = true;
    while (notSorted)
    {
        notSorted = false;

        auto sortPhase = [&](u8 i)
        {
            if (shadowOAM[i-1].fill > shadowOAM[i].fill)
            {
                notSorted = true;
                using std::swap;
                swap(shadowOAM[i-1], shadowOAM[i]);
                swap(idByPos[i-1], idByPos[i]);
                pos[idByPos[i-1]] = i-1;
                pos[idByPos[i]] = i;
            }
        };

        // Forward phase and backward phase
        for (u8 i = 1; i < objCount; i++) sortPhase(i);
        for (u8 i = objCount-1; i > 0; i--) sortPhase(i);
    }

    // Select the projectile position
    preProjPos = std::lower_bound(shadowOAM, shadowOAM+objCount, projectilePrio,
        [](const OBJ_ATTR& obj, int prio) { return (obj.fill & 127) < prio; }) - shadowOAM;
}
