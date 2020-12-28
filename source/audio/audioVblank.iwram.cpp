//--------------------------------------------------------------------------------
// audioVblank.iwram.cpp
//--------------------------------------------------------------------------------
// The audio's vblank should come here
//--------------------------------------------------------------------------------
#include "audioBuffers.hpp"

void audioVblank()
{
    // Start the timer
    REG_TM0CNT = TM_ENABLE;

    if (curFrame == 0)
    {
        // Restart the DMA
        REG_DMA1CNT = 0;
        REG_DMA1SAD = (u32)&audioMixBuffers[0][0];
        REG_DMA1CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

        curAudioMixBuffer = &audioMixBuffers[1][0];
        curFrame = 1;
    }
    else
    {
        // Don't need to restart the DMA
        curAudioMixBuffer = &audioMixBuffers[0][0];
        curFrame = 0;
    }
}
