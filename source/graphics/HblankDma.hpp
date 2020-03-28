//--------------------------------------------------------------------------------
// HblankDma.hpp
//--------------------------------------------------------------------------------
// Provides the class to manage the HDMA effects
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

class HblankDma final
{
    struct DmaCopy { const void* src; void* dst; u32 countCtl; };

    DmaCopy copies[2][SCREEN_HEIGHT+1];
    u32 curFrame;

public:
    void init();

    inline void addHdma32(u32 line, const void* src, void* dst, u32 numWords)
    {
        copies[curFrame][line].src = src;
        copies[curFrame][line].dst = dst;
        copies[curFrame][line].countCtl = numWords | DMA_32 | DMA_ENABLE;
    }

    inline void addHdma16(u32 line, const void* src, void* dst, u32 numHwords)
    {
        copies[curFrame][line].src = src;
        copies[curFrame][line].dst = dst;
        copies[curFrame][line].countCtl = numHwords | DMA_16 | DMA_ENABLE;
    }

    void vblank();
    void update();
};
