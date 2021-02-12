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
        REG_DMA1SAD = (u32)&audioMixBuffersLeft[0][0];
        REG_DMA1DAD = (u32)&REG_FIFO_A;
        REG_DMA1CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

        REG_DMA2CNT = 0;
        REG_DMA2SAD = (u32)&audioMixBuffersRight[0][0];
        REG_DMA2DAD = (u32)&REG_FIFO_B;
        REG_DMA2CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

        curAudioMixBufferLeft = &audioMixBuffersLeft[1][0];
        curAudioMixBufferRight = &audioMixBuffersRight[1][0];
        curFrame = 1;
    }
    else
    {
        // Don't need to restart the DMA
        curAudioMixBufferLeft = &audioMixBuffersLeft[0][0];
        curAudioMixBufferRight = &audioMixBuffersRight[0][0];
        curFrame = 0;
    }
}
