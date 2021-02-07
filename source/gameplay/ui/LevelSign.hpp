//--------------------------------------------------------------------------------
// LevelSign.hpp
//--------------------------------------------------------------------------------
// Provides an animated introduction to the current level
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

constexpr auto SignHeight = 56;

class LevelSign final
{
    u16 newBlendRegs[2];
    u16 restoreBlendRegister;

    u32 newDmaRegs[3];
    u32 restoreDmaRegister;
    s16 hofsUpdates[SignHeight + 1];
    s16 levelOffset;

    int numFrames;

public:
    LevelSign(int level);
    ~LevelSign();

    void vblank();
    void update();

    bool isComplete() const { return numFrames <= 0; }
};
