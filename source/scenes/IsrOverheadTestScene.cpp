//--------------------------------------------------------------------------------
// IsrOverheadTestScene.cpp
//--------------------------------------------------------------------------------
// Measures the overhead of the current interrupt service routine
//--------------------------------------------------------------------------------
#include "IsrOverheadTestScene.hpp"

#include "data/fonts/monogram_extended.hpp"
#include "colors.hpp"

#include <tonc.h>

volatile int cycleCount IWRAM_DATA;
extern "C" void hblankReportCycles() IWRAM_CODE;

void vcountInterrupt()
{
    // This -192 is experimental
    constexpr s16 CalculatedVal = 240 * 4 - 192;
    REG_TM2D = -CalculatedVal;
    REG_TM2CNT = TM_IRQ | TM_ENABLE;
}

IsrOverheadTestScene::IsrOverheadTestScene() : IScene(), writer(data::fonts::monogram_extended.ttf, vid_mem)
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;
    REG_DISPSTAT = (REG_DISPSTAT &~ DSTAT_VCT_MASK) | DSTAT_VCT(24);

    cycleCount = -1;
    irq_add(II_VCOUNT, vcountInterrupt);
    irq_add(II_TIMER2, hblankReportCycles);
}

void IsrOverheadTestScene::update()
{
    // Clear some lines
    memset32(vid_mem+240*80, 0, 240*20/2);

    StringBuilder<64> sb;
    sb.append("Available hblank cycles: ", cycleCount, " cycles.");
    writer.write(4, 96, sb, colors::LimeGreen);
}
