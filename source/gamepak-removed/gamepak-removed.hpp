//--------------------------------------------------------------------------------
// gamepak-removed.hpp
//--------------------------------------------------------------------------------
// The interrupt routine to be fired when the game pak is detected as removed
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

// This requires some modifications and tweaks to the linker file
void gamepakRemovedIsr() __attribute__((section(".ewram.gamepakisr"), long_call));

namespace gamepak
{
    static inline void setUpIsr()
    {
        irq_set(II_GAMEPAK, gamepakRemovedIsr, 0);
    }
}
