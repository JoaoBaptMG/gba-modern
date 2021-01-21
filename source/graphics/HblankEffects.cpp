//--------------------------------------------------------------------------------
// HblankEffects.cpp
//--------------------------------------------------------------------------------
// Provides the class to manage the H-Blank effects
//--------------------------------------------------------------------------------
#include "HblankEffects.hpp"

#include "util/gba-assert.hpp"

constexpr auto MaxEffects = 15;

struct DmaCopy { u32 src, dst, cnt; };

// The DMA copies and the lines
u16 hblankCurFrame, hblankCurIdx;
u8 hblankLines[2][MaxEffects+1];
DmaCopy hblankWrites[2][MaxEffects+1];
static u16 curMaxEffects[2];

// The interrupts to be set
extern "C"
{
    void updateEffectIsr() IWRAM_CODE;
}

void HblankEffects::init()
{
    // Initialize everything
    hblankLines[0][0] = 255;
    hblankLines[1][0] = 255;

    // Set up the interrupts - vcount at highest priority
    irq_set(II_VCOUNT, []{ REG_TM1CNT = TM_IRQ | TM_ENABLE; }, 0);
    irq_set(II_TIMER1, updateEffectIsr, 1);

    // Setup timer 1
    constexpr s16 TimerValue = SCREEN_WIDTH * 4 - 136;
    REG_TM1D = -TimerValue;
}

int HblankEffects::makeSpaceForEffect(int line)
{
    ASSERT(curMaxEffects[!hblankCurFrame] < MaxEffects);

    // Bubble sort the effects and add on its place to keep it sorted
    u32 i = curMaxEffects[!hblankCurFrame];
    while (i > 0 && hblankLines[!hblankCurFrame][i-1] > line)
    {
        hblankLines[!hblankCurFrame][i] = hblankLines[!hblankCurFrame][i-1];
        hblankWrites[!hblankCurFrame][i] = hblankWrites[!hblankCurFrame][i-1];
        i--;
    }
    curMaxEffects[!hblankCurFrame]++;

    return i;
}

void HblankEffects::add16(int line, const void* src, void* dst, u32 nhwords)
{
    int id = makeSpaceForEffect(line);
    hblankLines[!hblankCurFrame][id] = line;
    hblankWrites[!hblankCurFrame][id] = { (u32)src, (u32)dst, nhwords | DMA_16NOW | DMA_ENABLE };
}

void HblankEffects::add32(int line, const void* src, void* dst, u32 nwords)
{
    int id = makeSpaceForEffect(line);
    hblankLines[!hblankCurFrame][id] = line;
    hblankWrites[!hblankCurFrame][id] = { (u32)src, (u32)dst, nwords | DMA_32NOW | DMA_ENABLE };
}

void HblankEffects::flush()
{
    // Here, we're going to write a dummy line
    hblankLines[!hblankCurFrame][curMaxEffects[!hblankCurFrame]] = 255;
}

void HblankEffects::vblank()
{
    hblankCurFrame = !hblankCurFrame;

    // Here we work on the *current frame*
    hblankCurIdx = 0;
    int nextLine = hblankLines[hblankCurFrame][0];
    if (nextLine == 255) REG_DISPSTAT &= ~DSTAT_VCT_IRQ;
    else
    {
        nextLine = nextLine == 0 ? 227 : nextLine - 1;
        REG_DISPSTAT = (REG_DISPSTAT & ~DSTAT_VCT_MASK) | DSTAT_VCT_IRQ | DSTAT_VCT(nextLine);

        // Now, reset the other frame it to zero
        hblankLines[!hblankCurFrame][0] = 255;
        curMaxEffects[!hblankCurFrame] = 0;
    }
}
