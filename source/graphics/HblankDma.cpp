//--------------------------------------------------------------------------------
// HblankDma.cpp
//--------------------------------------------------------------------------------
// Provides the class to manage the HDMA effects
//--------------------------------------------------------------------------------
#include "HblankDma.hpp"

void HblankDma::init()
{
    // Set everything to zero
    memset32(copies, 0, sizeof(copies)/sizeof(u32));
}

void HblankDma::vblank()
{
    // Transfer the first line
    *(DmaCopy*)(&REG_DMA3SAD) = copies[curFrame][0];

    // Set up DMA0 to transfer for the next lines
    REG_DMA0SAD = (u32)&copies[curFrame][1];
    REG_DMA0DAD = (u32)&REG_DMA3SAD;

    // First, disable DMA0 then reenable it
    REG_DMA0CNT = 0;
    REG_DMA0CNT = 3 | DMA_DST_RELOAD | DMA_SRC_INC | DMA_REPEAT | DMA_32 | DMA_AT_HBLANK | DMA_ENABLE;
}

void HblankDma::update()
{
    curFrame ^= 1;
    memset32(&copies[curFrame], 0, sizeof(copies[0])/sizeof(u32));
}