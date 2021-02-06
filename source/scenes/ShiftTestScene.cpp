//--------------------------------------------------------------------------------
// ShiftTestScene.cpp
//--------------------------------------------------------------------------------
// <insert description here>
//--------------------------------------------------------------------------------
#include "ShiftTestScene.hpp"

#include "data/fonts/monogram_extended.hpp"
#include "colors.hpp"
#include "text/StringBuilder.hpp"

#include <tonc.h>

extern "C" u32 lslAsm(u32 a, u32 s) IWRAM_CODE;
extern "C" u32 lsrAsm(u32 a, u32 b) IWRAM_CODE;

ShiftTestScene::ShiftTestScene() : IScene(), writer(data::fonts::monogram_extended.ttf, vid_mem)
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    writer.write(4, 12, "Off-Range Shift Test", colors::DeepSkyBlue);

    StringBuilder<1024> sb;
    sb.append("240 lsl 0x3   = ", lslAsm(240, 0x3), '\n');
    sb.append("240 lsl 0x23  = ", lslAsm(240, 0x23), '\n');
    sb.append("240 lsl 0x43  = ", lslAsm(240, 0x43), '\n');
    sb.append("240 lsl 0x83  = ", lslAsm(240, 0x83), '\n');
    sb.append("240 lsl 0x103 = ", lslAsm(240, 0x103), "\n\n");

    sb.append("240 lsr 0x3   = ", lsrAsm(240, 0x3), '\n');
    sb.append("240 lsr 0x23  = ", lsrAsm(240, 0x23), '\n');
    sb.append("240 lsr 0x43  = ", lsrAsm(240, 0x43), '\n');
    sb.append("240 lsr 0x83  = ", lsrAsm(240, 0x83), '\n');
    sb.append("240 lsr 0x103 = ", lsrAsm(240, 0x103));

    writer.write(4, 36, sb, colors::White);
}