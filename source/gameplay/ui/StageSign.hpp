//--------------------------------------------------------------------------------
// StageSign.hpp
//--------------------------------------------------------------------------------
// Provides an animated introduction to the current stage
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

constexpr auto SignHeight = 48;

class StageSign final
{
    u16 newBlendRegs[2];
    u16 restoreBlendRegister;

    u32 newDmaRegs[3];
    u32 restoreDmaRegister;
    s16 hofsUpdates[SignHeight + 1];
    s16 stageOffset;

    s16 numFrames, curStage;

    void setStageScreenEntries();

public:
    StageSign(int stage);
    ~StageSign();

    void vblank();
    void update();

    bool isComplete() const { return numFrames <= 0; }
};
