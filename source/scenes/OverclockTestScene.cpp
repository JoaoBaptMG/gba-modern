//--------------------------------------------------------------------------------
// OverclockTestScene.cpp
//--------------------------------------------------------------------------------
// A test scene for testing the overclock register on different modes
//--------------------------------------------------------------------------------
#include "OverclockTestScene.hpp"

#include "data/fonts/monogram_extended.hpp"
#include "text/StringBuilder.hpp"

OverclockTestScene::OverclockTestScene() : IScene(), writer(data::fonts::monogram_extended, vid_mem)
{
    auto value = *(vu32*)0x4000800;

    // Set the display registers
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    StringBuilder<64> sb;
    sb.append("Value read from 4000800: 0x", hex(value));
    writer.write(4, 32, sb, CLR_WHITE);
}